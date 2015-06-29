#ifndef XMLBUILDER_H
#define XMLBUILDER_H

#include <LiteralXml.h>
#include <XmlSink.h>
#include <QDomDocument>
#include <QDebug>

template <typename Base = void, typename Tag = void>
class XmlBuilder {
public:
    QDomNode node;
    explicit XmlBuilder(const QDomNode& n) :node(n) {
        Q_ASSERT(!n.isNull());
    }
};

template <typename Base, typename Tag, typename ChildTag>
typename std::enable_if<ChildTag::is_tag,XmlBuilder<XmlBuilder<Base,Tag>, ChildTag>>::type
operator<(const XmlBuilder<Base,Tag>& b, const ChildTag& tag) {
    QDomNode node = b.node;
    QDomDocument doc = node.ownerDocument();
    QDomElement e = doc.createElementNS(tag.ns(), tag.name());
    node.appendChild(e);
    return XmlBuilder<XmlBuilder<Base,Tag>, ChildTag>(e);
}

template <typename Base, typename Tag, typename ChildTag>
typename std::enable_if<ChildTag::is_tag,XmlBuilder<XmlBuilder<Base,Tag>, ChildTag>>::type
operator<(const XmlBuilder<Base,Tag>& b, const ElementStart<ChildTag>& es) {
    QDomNode node = b.node;
    QDomDocument doc = node.ownerDocument();
    QDomElement e = doc.createElementNS(es.qname.ns(), es.qname.name());
    for (const AttributeNode& a: es.atts) {
        e.setAttributeNS(a.qname.ns(), a.qname.name(), a.value);
    }
    node.appendChild(e);
    return XmlBuilder<XmlBuilder<Base,Tag>, ChildTag>(e);
}

template <typename Base, typename Tag>
Base operator>(const XmlBuilder<Base,Tag>& b, const Tag&) {
    return Base(b.node.parentNode());
}

template <typename Base, typename Tag>
XmlBuilder<Base,Tag> operator<(const XmlBuilder<Base,Tag>& b, const char* val) {
    QDomNode node = b.node;
    QDomDocument doc = node.ownerDocument();
    node.appendChild(doc.createTextNode(val));
    return b;
}

template <typename Base, typename Tag>
XmlBuilder<Base,Tag> operator<(const XmlBuilder<Base,Tag>& b, const QString& val) {
    QDomNode node = b.node;
    QDomDocument doc = node.ownerDocument();
    node.appendChild(doc.createTextNode(val));
    return b;
}

template<typename F, typename Base, typename Tag>
auto operator<(XmlBuilder<Base,Tag> w, F f) -> decltype(f(w)) {
    return f(w);
}

template<typename Base, typename Tag>
XmlBuilder<Base,Tag> operator<(XmlBuilder<Base,Tag> w, XmlBuilder<Base,Tag> (*f)(XmlBuilder<Base,Tag>)) {
    return f(w);
}

#endif // XMLBUILDER_H
