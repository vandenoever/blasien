#ifndef LITERALXML_H
#define LITERALXML_H

struct AttributeNode;

struct QName {
    const QString ns;
    const QString name;
    QName(const QString& ns_, const QString& name_) :ns(ns_), name(name_) {}
    AttributeNode operator()(const QString& val) const;
    AttributeNode operator=(const QString& val) const;
};

struct AttributeNode {
    const QName qname;
    const QString value;
    AttributeNode(const QName& q, const QString& v) :qname(q), value(v) {}
};


AttributeNode QName::operator()(const QString& val) const {
    return AttributeNode(*this, val);
}
AttributeNode QName::operator=(const QString& val) const {
    return AttributeNode(*this, val);
}

#endif // LITERALXML_H
