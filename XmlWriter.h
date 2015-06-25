#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <QXmlStreamWriter>
#include <LiteralXml.h>

template <typename Base = int, typename Tag = int>
class XmlWriter {
public:
    QXmlStreamWriter& writer;
    explicit XmlWriter(QXmlStreamWriter& w) :writer(w) {}
};

template <typename Base, typename Tag, typename ChildTag>
typename std::enable_if<std::is_base_of<QName,ChildTag>::value,XmlWriter<XmlWriter<Base,Tag>, ChildTag>>::type
operator<(const XmlWriter<Base,Tag>& w, const ChildTag& tag) {
    w.writer.writeStartElement(tag.ns, tag.name);
    return XmlWriter<XmlWriter<Base,Tag>, ChildTag>(w.writer);
}

template <typename Base, typename Tag>
Base operator>(const XmlWriter<Base,Tag>& w, const Tag&) {
    w.writer.writeEndElement();
    return Base(w.writer);
}

template <typename Base, typename Tag>
XmlWriter<Base,Tag> operator<(XmlWriter<Base,Tag>, const char*) {
    return XmlWriter<Base,Tag>();
}

#endif // XMLWRITER_H
