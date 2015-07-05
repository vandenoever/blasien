#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <QXmlStreamWriter>
#include <XmlSink.h>

template <typename NodeType_>
class XmlWriter {
public:
    static constexpr bool is_xmlsink = true;
    using NodeType = NodeType_;
    using StringType = QString;
    QXmlStreamWriter& writer;
    explicit XmlWriter(QXmlStreamWriter& w) :writer(w) {}
    template <typename Tag>
    inline void startElement(const Tag &tag) const {
        writer.writeStartElement(tag.ns(), tag.name());
    }
    inline void endElement() const {
        writer.writeEndElement();
    }
    template <typename Tag>
    inline void writeAttribute(const Tag &tag, const QString& value) const {
        writer.writeAttribute(tag.ns(), tag.name(), value);
    }
    inline void writeCharacters(const QString& value) const {
        writer.writeCharacters(value);
    }
};

#endif // XMLWRITER_H
