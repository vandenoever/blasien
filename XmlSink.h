#ifndef XMLSINK_H
#define XMLSINK_H

template <typename Base = void, typename Tag = void>
class XmlSink {
public:
    static constexpr bool is_xmlsink = true;
    const Base& base;
    XmlSink(const Base& b) :base(b) {}
    template <typename ChildTag>
    inline void startElement(const ChildTag& tag) const {
        base.startElement(tag);
    }
    inline void endElement() const {
        base.endElement();
    }
    template <typename ChildTag>
    inline void writeAttribute(const ChildTag& tag, const QString& value) const {
        base.writeAttribute(tag, value);
    }
    inline void writeCharacters(const QString& val) const {
        base.writeCharacters(val);
    }
};

template <typename Sink, typename Tag>
typename std::enable_if<Sink::is_xmlsink && Tag::is_tag,XmlSink<Sink,Tag>>::type
operator<(const Sink& sink, const Tag& tag) {
    sink.startElement(tag);
    return XmlSink<Sink, Tag>(sink);
}

template <typename Sink, typename Tag>
typename std::enable_if<Sink::is_xmlsink && Tag::is_tag,XmlSink<Sink, Tag>>::type
operator<(const Sink& sink, const ElementStart<Tag>& e) {
    sink.startElement(e.qname);
    for (const AttributeNode& a: e.atts) {
        sink.writeAttribute(a.qname, a.value);
    }
    return XmlSink<Sink, Tag>(sink);
}

template <typename Base, typename Tag>
Base operator>(const XmlSink<Base,Tag>& sink, const Tag&) {
    sink.endElement();
    return sink.base;
}

template <typename Base, typename Tag>
XmlSink<Base,Tag> operator<(const XmlSink<Base,Tag>& sink, const char* val) {
    sink.writeCharacters(val);
    return sink;
}

template <typename Base, typename Tag>
XmlSink<Base,Tag> operator<(const XmlSink<Base,Tag>& sink, const QString& val) {
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
