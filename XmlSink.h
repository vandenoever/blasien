#ifndef XMLSINK_H
#define XMLSINK_H

#include <tuple>

struct TextType;
struct AnyType;

template <typename NodeType>
struct allowed_child_types;

template <>
struct allowed_child_types<AnyType> {
    using types = std::tuple<AnyType,TextType>;
};

template <>
struct allowed_child_types<TextType> {
    using types = std::tuple<>;
};

template <typename Base, typename NodeType_>
class XmlSink {
public:
    static constexpr bool is_xmlsink = true;
    using NodeType = NodeType_;
    using StringType = typename Base::StringType;
    const Base& base;
    inline XmlSink(const Base& b) :base(b) {}
    template <typename ChildTag>
    inline void startElement(const ChildTag& tag) const {
        base.startElement(tag);
    }
    inline void endElement() const {
        base.endElement();
    }
    template <typename ChildTag, typename String>
    inline void writeAttribute(const ChildTag& tag, const String& value) const {
        base.writeAttribute(tag, value);
    }
    template <typename String>
    inline void writeCharacters(const String& val) const {
        base.writeCharacters(val);
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

template <typename Sink, typename Tag>
typename std::enable_if<Sink::is_xmlsink && Tag::is_tag,typename type_from_tag<Sink,Tag>::type>::type
operator<(const Sink& sink, const Tag& tag) {
    sink.startElement(tag);
    return typename type_from_tag<Sink,Tag>::type(sink);
}

template <typename Type>
struct is_tuple;

template <typename Type>
struct is_tuple : std::false_type {};

template <typename... Types>
struct is_tuple<std::tuple<Types...>> : std::true_type {};
/*
template <typename T, typename RequiredAttributes>
struct required_attributes {
    typename std::enable_if<
};*/

template <typename NodeType>
constexpr auto has_required_attributes(NodeType& t) -> decltype(NodeType::requiredAttributes, bool()) {
    return true;
}

constexpr bool has_required_attributes(...) { return false; }

template <typename NodeType, bool hasRequiredAttributes>
struct required_attributes;

template <typename NodeType>
struct required_attributes<NodeType, true> {
    using type = typename NodeType::RequiredAttributes;
};

template <typename NodeType>
struct required_attributes<NodeType, false> {
    using type = std::tuple<>;
};

template <size_t N, typename SetAttributes = std::tuple<>, typename SetReqAttributes = std::tuple<>>
struct write_attributes {
    template<typename Sink, typename... T>
    typename std::enable_if<N!=sizeof...(T),void>::type
    write(const Sink& sink, const std::tuple<T...>& t) const {
        using XmlTag = typename std::remove_reference<decltype(std::get<N>(t))>::type::XmlTag;
        sink.writeAttribute(XmlTag::qname,std::get<N>(t).value);
        using SA = typename tuple_prepend<XmlTag, SetAttributes>::type;
        write_attributes<N+1, SA>().write(sink, t);
    }
    template<typename Sink, typename... T>
    typename std::enable_if<N==sizeof...(T),void>::type
    write(const Sink&, const std::tuple<T...>&) const {
        // todo: check that all required attributes have been set
        static_assert(is_tuple<SetReqAttributes>::value, "SetReqAttributes must be a std::tuple.");
        using NodeType = typename Sink::NodeType;
        using RequiredAttributes = typename required_attributes<NodeType, has_required_attributes(NodeType())>::type;
        static_assert(is_tuple<RequiredAttributes>::value, "RequiredAttributes must be a std::tuple.");
        const int setReqAttributes = std::tuple_size<SetReqAttributes>::value;
        const int reqAttributes = std::tuple_size<RequiredAttributes>::value;
        static_assert(setReqAttributes == reqAttributes, "not all required attributes have been set.");
    }
};

template <typename Sink, typename Tag, typename... Atts>
typename type_from_tag<Sink,Tag>::type
operator<(const Sink& sink, const ElementStart<Tag, Atts...>& e) {
    sink.startElement(Tag::qname);
    write_attributes<0>().write(sink, e.atts);
    return typename type_from_tag<Sink,Tag>::type(sink);
}

template <typename Base, typename Type>
Base operator>(const XmlSink<Base,Type>& sink, const typename Type::Tag&) {
    sink.endElement();
    return sink.base;
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
    static_assert(!std::is_same<Sink,QString>(),"ohoh");
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

#endif
