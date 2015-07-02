#ifndef LITERALXML_H
#define LITERALXML_H

struct AttributeNode;
template <typename Tag>
struct ElementStart;


struct QName {
    static const bool is_tag = true;
    QString ns_;
    QString name_;
    QName(const QString& ns, const QString& name) :ns_(ns), name_(name) {}
    const QString& ns() const { return ns_; }
    const QString& name() const { return name_; }
    template <typename ElementStart>
    ElementStart addAttributes(std::initializer_list<AttributeNode>) const;
};

// represents an xpath Step which consists of a NameTest qualified by a Predicate
template <typename NameTestTag, typename Predicate>
struct NameTestWithPredicate {
    const NameTestTag tag;
    const Predicate predicate;
    NameTestWithPredicate(const NameTestTag& testTag, const Predicate& p) :tag(testTag), predicate(p) {}
};

template <typename Tag>
struct AttributeName {
    const Tag tag;
    AttributeName(const Tag& t) :tag(t) {}
};

template <const QString* Ns, const QString* Name>
struct XmlTag {
    using Self = XmlTag<Ns, Name>;
    static constexpr bool is_tag = true;
    static const QName qname;
    static const QString& ns() {
        return qname.ns();
    }
    static const QString& name() {
        return qname.name();
    }
    ElementStart<Self>
    operator()(std::initializer_list<AttributeNode> atts) const {
        return ElementStart<Self>(qname, atts);
    }
    template <typename Predicate>
    NameTestWithPredicate<Self, Predicate>
    operator[](const Predicate& p) const {
        return NameTestWithPredicate<Self,Predicate>(*this, p);
    }
    AttributeName<Self> operator~() const {
        return AttributeName<Self>(*this);
    }
    // set the attribute
    AttributeNode operator=(const QString& val) const;
};

template <const QString* Ns, const QString* Name>
const QName XmlTag<Ns, Name>::qname(*Ns, *Name);

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

template <const QString* Ns, const QString* Name>
AttributeNode
XmlTag<Ns,Name>::operator=(const QString& val) const {
    return AttributeNode(qname, val);
}

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
