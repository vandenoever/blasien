#ifndef XMLSINK_H
#define XMLSINK_H

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

template <typename Sink, typename Tag>
typename std::enable_if<Sink::is_xmlsink && Tag::is_tag,typename type_from_tag<Sink,Tag>::type>::type
operator<(const Sink& sink, const Tag& tag) {
    sink.startElement(tag);
    return typename type_from_tag<Sink,Tag>::type(sink);
}

template <typename Sink, typename Tag>
typename type_from_tag<Sink,Tag>::type
operator<(const Sink& sink, const ElementStart<Tag>& e) {
    sink.startElement(Tag::qname);
    for (const AttributeNode<typename Tag::String>& a: e.atts) {
        sink.writeAttribute(a.qname, a.value);
    }
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

template<typename F, typename Sink>
auto operator<(const Sink& sink, F f) -> decltype(f(sink)) {
    return f(sink);
}

template<typename Sink>
Sink operator<(const Sink& sink, Sink (*f)(const Sink&)) {
    return f(sink);
}

#endif
