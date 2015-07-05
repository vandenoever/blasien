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
