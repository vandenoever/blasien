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
#ifndef XMLBUILDER_H
#define XMLBUILDER_H

#include <XmlSink.h>
#include <QDomDocument>

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
