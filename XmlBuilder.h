#ifndef XMLBUILDER_H
#define XMLBUILDER_H

#include <LiteralXml.h>
#include <XmlSink.h>
#include <QDomDocument>
#include <QDebug>

template <typename NodeType_>
class XmlBuilder {
public:
    static constexpr bool is_xmlsink = true;
    using NodeType = NodeType_;
    using StringType = QString;
    QDomDocument doc;
    QDomNode& node;
    explicit XmlBuilder(QDomNode& n) :node(n) {
        Q_ASSERT(!node.isNull());
    }
    explicit XmlBuilder(QDomDocument& d) :doc(d), node(d) {
        Q_ASSERT(!d.isNull());
    }
    template <typename Tag>
    void startElement(const Tag &tag) const {
        QDomDocument doc = node.ownerDocument();
        QDomElement e = doc.createElementNS(tag.ns(), tag.name());
        node = node.appendChild(e);
    }
    inline void endElement() const {
        node = node.parentNode();
    }
    template <typename Tag>
    inline void writeAttribute(const Tag &tag, const QString& value) const {
        node.toElement().setAttributeNS(tag.ns(), tag.name(), value);
    }
    inline void writeCharacters(const QString& value) const {
        node.appendChild(node.ownerDocument().createTextNode(value));
    }
};

#endif // XMLBUILDER_H
