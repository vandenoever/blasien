#ifndef XMLPATH_H
#define XMLPATH_H

#include <QDomDocument>

/*
template <typename Tag1, typename Tag2>
typename std::enable_if<Tag1::is_tag && Tag2::is_tag,XPathOr>::type
operator|(const Tag1& tag1, const Tag2) {

}
*/

template <typename Tag>
std::list<QDomNode>
operator/(const QDomNode& node, const Tag& tag) {
    std::list<QDomNode> list;
    auto n = node.firstChild();
    while (!n.isNull()) {
        if (n.nodeType() == QDomNode::ElementNode && tag.ns() == n.namespaceURI() && tag.name() == n.localName()) {
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
