#ifndef LITERALXML_H
#define LITERALXML_H

struct AttributeNode;
template <typename Tag>
struct ElementStart;

struct QName {
    QString ns;
    QString name;
    QName(const QString& ns_, const QString& name_) :ns(ns_), name(name_) {}
    template <typename ElementStart>
    ElementStart addAttributes(std::initializer_list<AttributeNode>) const;
};

struct AttributeNode {
    QName qname;
    QString value;
    AttributeNode(const QName& q, const QString& v) :qname(q), value(v) {}
    const AttributeNode& operator=(const AttributeNode& a) {
        qname = a.qname;
        value = a.value;
        return a;
    }
};

template <typename ElementStart>
ElementStart QName::addAttributes(std::initializer_list<AttributeNode>) const {
    return ElementStart(*this, QList<AttributeNode>());
}

template <typename Tag>
struct ElementStart {
    const QName qname;
    const QList<AttributeNode> atts;
    ElementStart(const QName& q, const QList<AttributeNode>& l) :qname(q), atts(l) {}
};

#endif // LITERALXML_H
