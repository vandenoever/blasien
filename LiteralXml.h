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

template <typename Filter, typename SubFilter>
struct XmlFilter {
    const Filter filter;
    const SubFilter subFilter;
    XmlFilter(const Filter& filter_, const SubFilter& subFilter_) :filter(filter_), subFilter(subFilter_) {}
};

template <const QString* Ns, const QString* Name, bool isAttribute=true, bool isElement=true>
struct XmlTag {
    using Self = XmlTag<Ns, Name, isAttribute, isElement>;
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
    template <typename T>
    XmlFilter<Self,T>
    operator[](const T& t) const {
        return XmlFilter<Self,T>(*this, t);
    }
    // set the attribute
    AttributeNode operator=(const QString& val) const;
};

template <const QString* Ns, const QString* Name, bool isAttribute, bool isElement>
const QName XmlTag<Ns, Name, isAttribute,isElement>::qname(*Ns, *Name);

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

template <const QString* Ns, const QString* Name, bool isAttribute, bool isElement>
AttributeNode
XmlTag<Ns,Name,isAttribute,isElement>::operator=(const QString& val) const {
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
