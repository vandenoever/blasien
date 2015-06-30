#ifndef XMLPATH_H
#define XMLPATH_H

#include <utility>

#include <QDomDocument>


template <typename Tag1, typename Tag2>
typename std::enable_if<Tag1::is_tag && Tag2::is_tag,std::pair<Tag1,Tag2>>::type
operator|(const Tag1& tag1, const Tag2& tag2) {
    return std::make_pair(tag1, tag2);
}

template <typename Tag>
typename std::enable_if<Tag::is_tag,bool>::type
node_filter_matches(const QDomNode& n, const Tag& tag) {
    return n.nodeType() == QDomNode::ElementNode && tag.ns() == n.namespaceURI() && tag.name() == n.localName();
}

template <typename A, typename B>
bool
node_filter_matches(const QDomNode& n, const std::pair<A,B>& p) {
    return node_filter_matches(n, p.first) || node_filter_matches(n, p.second);
}

template <typename Tag>
std::list<QDomNode>
operator/(const QDomNode& node, const Tag& tag) {
    std::list<QDomNode> list;
    QDomNode n = node.firstChild();
    while (!n.isNull()) {
        if (node_filter_matches(n, tag)) {
            list.push_back(n);
        }
        n = n.nextSibling();
    }
    return list;
}

template <typename Tag>
std::list<QDomNode>
operator/(const std::list<QDomNode>& nodes, const Tag& tag) {
    std::list<QDomNode> list;
    for (const auto& l: nodes) {
        auto i = l/tag;
        for (const auto& n: i) {
            list.push_back(n);
        }
    }
    return list;
}




#endif
