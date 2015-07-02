#ifndef LITERALXML_H
#define LITERALXML_H

template <typename String>
struct AttributeNode;
template <typename Tag>
struct ElementStart;

template <typename String_>
struct QName {
    using String = String_;
    static const bool is_tag = true;
    String ns_;
    String name_;
    QName(const String& ns, const String& name) :ns_(ns), name_(name) {}
    const String& ns() const { return ns_; }
    const String& name() const { return name_; }
    template <typename ElementStart>
    ElementStart addAttributes(std::initializer_list<AttributeNode<String>>) const;
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

template <typename String_, const String_* Ns, const String_* Name>
struct XmlTag {
    using String = String_;
    using Self = XmlTag<String,Ns, Name>;
    static constexpr bool is_tag = true;
    static const QName<String> qname;
    static const String& ns() {
        return qname.ns();
    }
    static const String& name() {
        return qname.name();
    }
    ElementStart<Self>
    operator()(std::initializer_list<AttributeNode<String>> atts) const {
        return ElementStart<Self>(atts);
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
    AttributeNode<String> operator=(const String& val) const;
};

template <typename String, const String* Ns, const String* Name>
const QName<String> XmlTag<String, Ns, Name>::qname(*Ns, *Name);

template <typename String>
struct AttributeNode {
    QName<String> qname;
    String value;
    AttributeNode(const QName<String>& q, const String& v) :qname(q), value(v) {}
    const AttributeNode& operator=(const AttributeNode& a) {
        qname = a.qname;
        value = a.value;
        return a;
    }
};

template <typename String, const String* Ns, const String* Name>
AttributeNode<String>
XmlTag<String,Ns,Name>::operator=(const String& val) const {
    return AttributeNode<String>(qname, val);
}

template <typename String>
template <typename ElementStart>
ElementStart QName<String>::addAttributes(std::initializer_list<AttributeNode<String>>) const {
    return ElementStart(*this, QList<AttributeNode<String>>());
}

template <typename Tag_>
struct ElementStart {
    using Tag = Tag_;
    using String = typename Tag::String;
    const QList<AttributeNode<String>> atts;
    ElementStart(const QList<AttributeNode<String>>& l) :atts(l) {}
};

#endif // LITERALXML_H
