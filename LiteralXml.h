#ifndef LITERALXML_H
#define LITERALXML_H

template <typename Tag, typename... Atts>
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

template <typename XmlTag_>
struct AttributeNode {
    using XmlTag = XmlTag_;
    using String = typename XmlTag::String;
    String value;
    AttributeNode(const String& v) :value(v) {}
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
    template <typename... Atts>
    ElementStart<Self, Atts...>
    operator()(Atts... atts) const {
        return ElementStart<Self,Atts...>(atts...);
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
    AttributeNode<Self> operator=(const String& val) const {
        return AttributeNode<Self>(val);
    }
};

template <typename String, const String* Ns, const String* Name>
const QName<String> XmlTag<String, Ns, Name>::qname(*Ns, *Name);

template <typename Tag_, typename... Atts>
struct ElementStart {
    using Tag = Tag_;
    const std::tuple<Atts...> atts;
    ElementStart(Atts... atts_) :atts(atts_...) {}
};

#endif // LITERALXML_H
