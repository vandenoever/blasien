/****************************************************************************
**
** Copyright (C) 2015 Jos van den Oever
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 3 as published by the Free Software Foundation and appearing
** in the file lgpl-3.0.txt included in the packaging of this file.  Please
** review the following information to ensure the GNU Lesser General Public
** License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/
#ifndef XMLSINK_H
#define XMLSINK_H

#include <tuple>
#include <LiteralXml.h>

template <typename Base, typename NodeType_>
class XmlSink {
public:
    static constexpr bool is_xmlsink = true;
    using NodeType = NodeType_;
    using StringType = typename Base::StringType;
    const Base* basePtr;
    const Base& base() const { return *basePtr; }
    inline XmlSink(const Base& b) :basePtr(&b) {}
    template <typename ChildTag>
    inline void startElement(const ChildTag& tag) const {
        basePtr->startElement(tag);
    }
    inline void endElement() const {
        basePtr->endElement();
    }
    template <typename ChildTag, typename String>
    inline void writeAttribute(const ChildTag& tag, const String& value) const {
        basePtr->writeAttribute(tag, value);
    }
    template <typename String>
    inline void writeCharacters(const String& val) const {
        basePtr->writeCharacters(val);
    }
};

/* helper structs */
template <typename Tag, typename Types>
struct type_with_tag;

template <typename Type, typename... Types>
struct type_with_tag<typename Type::Tag, std::tuple<Type, Types...>> {
    using type = Type;
};

template <typename Tag, typename Type, typename... Types>
struct type_with_tag<Tag, std::tuple<Type, Types...>> {
    using type = typename type_with_tag<Tag, std::tuple<Types...>>::type;
};

template <typename Tag>
struct type_with_tag<Tag, std::tuple<>> {
    using type = void; // signal error
};

template <typename Sink, typename Tag>
struct type_from_tag {
    using types = typename allowed_child_types<typename Sink::NodeType>::types;
    using type = XmlSink<Sink,typename type_with_tag<Tag,types>::type>;
};


template <typename Type, typename Tuple>
struct tuple_prepend;

template <typename Type, typename... Types>
struct tuple_prepend<Type, std::tuple<Types...>> {
    using type = typename std::tuple<Type, Types...>;
};

template <typename Type>
struct is_tuple;

template <typename Type>
struct is_tuple : std::false_type {};

template <typename... Types>
struct is_tuple<std::tuple<Types...>> : std::true_type {};

template <typename NodeType>
constexpr auto has_required_attributes(const NodeType&) -> decltype(typename NodeType::requiredAttributes(), bool()) {
    return true;
}

constexpr bool has_required_attributes(...) { return false; }

template <typename NodeType, bool hasRequiredAttributes>
struct required_attributes;

template <typename NodeType>
struct required_attributes<NodeType, true> {
    using type = typename NodeType::requiredAttributes;
};

template <typename NodeType>
struct required_attributes<NodeType, false> {
    using type = std::tuple<>;
};

template <typename NodeType>
constexpr auto has_allowed_attributes(const NodeType&) -> decltype(typename NodeType::allowedAttributes(), bool()) {
    return true;
}

template <typename NodeType>
constexpr bool has_allowed_attributes(...) { return false; }

template <typename NodeType, bool hasAllowedAttributes>
struct allowed_attributes;

template <typename NodeType>
struct allowed_attributes<NodeType, true> {
    using type = typename NodeType::allowedAttributes;
};

template <typename NodeType>
struct allowed_attributes<NodeType, false> {
    using type = std::tuple<>;
};

template <typename Type, typename Tuple>
struct tuple_contains_type;

template <typename Type, typename... Types>
struct tuple_contains_type<Type, std::tuple<Type, Types...>> {
    static const bool value = true;
};

template <typename Type, typename T, typename... Types>
struct tuple_contains_type<Type, std::tuple<T, Types...>> {
    static const bool value = tuple_contains_type<Type, std::tuple<Types...>>::value;
};

template <typename Type>
struct tuple_contains_type<Type, std::tuple<>> {
    static const bool value = false;
};

template <size_t N, typename NodeType, typename SetAttributes = std::tuple<>, typename SetReqAttributes = std::tuple<>>
struct write_attributes {
    using AllowedAttributes = typename allowed_attributes<NodeType, has_allowed_attributes(NodeType())>::type;
    using RequiredAttributes = typename required_attributes<NodeType, has_required_attributes(NodeType())>::type;
    static_assert(is_tuple<SetAttributes>::value, "SetAttributes must be a std::tuple.");
    static_assert(is_tuple<SetReqAttributes>::value, "SetReqAttributes must be a std::tuple.");
    static_assert(is_tuple<AllowedAttributes>::value, "AllowedAttributes must be a std::tuple.");
    static_assert(is_tuple<RequiredAttributes>::value, "RequiredAttributes must be a std::tuple.");

    template<typename Sink, typename... T>
    typename std::enable_if<N!=sizeof...(T),void>::type
    write(const Sink& sink, const std::tuple<T...>& t) const {
        using Attribute = typename std::remove_reference<decltype(std::get<N>(t))>::type;
        using XmlTag = typename Attribute::XmlTag;
        static_assert(tuple_contains_type<XmlTag,AllowedAttributes>::value
                      || tuple_contains_type<XmlTag,RequiredAttributes>::value, "Attribute is not allowed here.");
        static_assert(!tuple_contains_type<XmlTag,SetAttributes>::value, "Attribute was already set on this element.");

        sink.writeAttribute(XmlTag::qname,std::get<N>(t).value);

        using SA = typename tuple_prepend<XmlTag, SetAttributes>::type;
        using A = typename tuple_prepend<XmlTag, SetReqAttributes>::type;
        static const bool attIsRequired = tuple_contains_type<XmlTag,RequiredAttributes>::value;
        using SRA = typename std::conditional<attIsRequired,A,SetReqAttributes>::type;
        write_attributes<N+1, NodeType, SA, SRA>().write(sink, t);
    }
    template<typename Sink, typename... T>
    typename std::enable_if<N==sizeof...(T),void>::type
    write(const Sink&, const std::tuple<T...>&) const {
        const int setReqAttributes = std::tuple_size<SetReqAttributes>::value;
        const int reqAttributes = std::tuple_size<RequiredAttributes>::value;
        static_assert(setReqAttributes <= reqAttributes, "Implementation error.");
        static_assert(setReqAttributes == reqAttributes, "Not all required attributes have been set.");
    }
};

// print a not completely incomprehensible error messages when nesting
// in the wrong way.
template <typename Parent, typename Child, typename ReturnType>
constexpr void check_nesting() {
    static_assert(!std::is_void<ReturnType>::value, "Nesting of Child element in Parent element is not allowed.");
}

template <typename Sink, typename Tag>
typename std::enable_if<Sink::is_xmlsink && Tag::is_tag,typename type_from_tag<Sink,Tag>::type>::type
operator<(const Sink& sink, const Tag& tag) {
    using ReturnType = typename type_from_tag<Sink,Tag>::type;
    check_nesting<typename Sink::NodeType, Tag, typename ReturnType::NodeType>();
    sink.startElement(tag);
    using NewSink = typename type_from_tag<Sink,Tag>::type;
    using NodeType = typename NewSink::NodeType;
    using RequiredAttributes = typename required_attributes<NodeType, has_required_attributes(NodeType())>::type;
    const int reqAttributes = std::tuple_size<RequiredAttributes>::value;
    static_assert(reqAttributes == 0, "Not all required attributes have been set.");
    return ReturnType(sink);
}

template <typename Sink, typename Tag, typename... Atts>
typename type_from_tag<Sink,Tag>::type
operator<(const Sink& sink, const ElementStart<Tag, Atts...>& e) {
    sink.startElement(Tag::qname);
    using NewSink = typename type_from_tag<Sink,Tag>::type;
    using NodeType = typename NewSink::NodeType;
    write_attributes<0, NodeType>().write(sink, e.atts);
    return NewSink(sink);
}

template <typename Base, typename Type>
Base operator>(const XmlSink<Base,Type>& sink, const typename Type::Tag&) {
    sink.endElement();
    return sink.base();
}

template <typename T, typename Types>
struct type_in_types;

template <typename T>
struct type_in_types<T, std::tuple<>> {
    static const bool value = false;
};

template <typename T, typename Type, typename... Types>
struct type_in_types<T, std::tuple<Type, Types...>> {
    static const bool value = type_in_types<T, std::tuple<Types...>>::value;
};

template <typename T, typename... Types>
struct type_in_types<T, std::tuple<T, Types...>> {
    static const bool value = true;
};

template <typename Sink, bool isSink>
struct sink_allows_text;

template <typename Sink>
struct sink_allows_text<Sink, false>
{
    static const bool value = false;
};

template <typename Sink>
struct sink_allows_text<Sink, true> {
    using types = typename allowed_child_types<typename Sink::NodeType>::types;
    static const bool value = type_in_types<TextType, types>::value;
};

template <typename Sink>
typename std::enable_if<sink_allows_text<Sink,Sink::is_xmlsink>::value, Sink>::type
operator<(const Sink& sink, const typename Sink::StringType& val) {
    sink.writeCharacters(val);
    return sink;
}

template<typename F, typename Sink, typename R = typename std::result_of<F(Sink)>::type>
typename std::enable_if<std::is_same<R,Sink>::value,Sink>::type
operator<(const Sink& sink, F f) {
    return f(sink);
}

template<typename Sink>
Sink operator<(const Sink& sink, Sink (*f)(const Sink&)) {
    return f(sink);
}

/**
 * Helper functor for the for_each function.
 */
template <typename Items, typename Lambda>
struct ForEach {
    const Items& items;
    Lambda lambda;
    template <typename Sink>
    Sink operator()(const Sink& sink) {
        auto s = sink;
        for (auto i: items) {
            s = lambda(s, i);
        }
        return s;
    }
};

/**
 * Create a functor that calls the lambda function for each item in the
 * collection. The lambda function creates XML that is passed into the
 * sink.
 */
template <typename Items, typename Lambda>
ForEach<Items, Lambda>
for_each(const Items& items, Lambda lambda) {
    return ForEach<Items,Lambda>{items, lambda};
}

#endif
