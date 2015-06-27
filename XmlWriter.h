#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <QXmlStreamWriter>
#include <LiteralXml.h>

template <typename Base = void, typename Tag = void>
class XmlWriter {
public:
    QXmlStreamWriter& writer;
    explicit XmlWriter(QXmlStreamWriter& w) :writer(w) {}
};

template <typename Base, typename Tag, typename ChildTag>
typename std::enable_if<ChildTag::is_tag,XmlWriter<XmlWriter<Base,Tag>, ChildTag>>::type
operator<(const XmlWriter<Base,Tag>& w, const ChildTag& tag) {
    w.writer.writeStartElement(tag.ns(), tag.name());
    return XmlWriter<XmlWriter<Base,Tag>, ChildTag>(w.writer);
}

template <typename Base, typename Tag, typename ChildTag>
typename std::enable_if<ChildTag::is_tag,XmlWriter<XmlWriter<Base,Tag>, ChildTag>>::type
operator<(const XmlWriter<Base,Tag>& w, const ElementStart<ChildTag>& e) {
    w.writer.writeStartElement(e.qname.ns(), e.qname.name());
    for (const AttributeNode& a: e.atts) {
        w.writer.writeAttribute(a.qname.ns(), a.qname.name(), a.value);
    }
    return XmlWriter<XmlWriter<Base,Tag>, ChildTag>(w.writer);
}

template <typename Base, typename Tag>
Base operator>(const XmlWriter<Base,Tag>& w, const Tag&) {
    w.writer.writeEndElement();
    return Base(w.writer);
}

template <typename Base, typename Tag>
XmlWriter<Base,Tag> operator<(const XmlWriter<Base,Tag>& w, const char* val) {
    w.writer.writeCharacters(val);
    return XmlWriter<Base,Tag>(w.writer);
}

template <typename Base, typename Tag>
XmlWriter<Base,Tag> operator<(const XmlWriter<Base,Tag>& w, const QString& val) {
    w.writer.writeCharacters(val);
    return XmlWriter<Base,Tag>(w.writer);
}

template<typename F, typename Base, typename Tag>
auto operator<(XmlWriter<Base,Tag> w, F f) -> decltype(f(w)) {
    return f(w);
}

template<typename Base, typename Tag>
XmlWriter<Base,Tag> operator<(XmlWriter<Base,Tag> w, XmlWriter<Base,Tag> (*f)(XmlWriter<Base,Tag>)) {
    return f(w);
}

#endif // XMLWRITER_H
