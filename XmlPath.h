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
#ifndef XMLPATH_H
#define XMLPATH_H

#include <LiteralXml.h>
#include <utility>

#include <QDomDocument>

// TODO: use terminology NodeTest (e.g. NameTest, NodeType) and Predicate

template <typename Predicate>
struct NotPredicate {
    const Predicate predicate;
    NotPredicate(const Predicate& p) :predicate(p) {}
};

template <typename Tag>
NotPredicate<AttributeName<Tag>>
operator!(const AttributeName<Tag>& a) {
    return NotPredicate<AttributeName<Tag>>(a);
}

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

template <typename Tag>
bool
node_filter_matches(const QDomNode& n, const AttributeName<Tag>& a) {
    return n.nodeType() == QDomNode::AttributeNode && a.tag.ns() == n.namespaceURI() && a.tag.name() == n.localName();
}

template <typename A, typename Predicate>
bool
node_filter_matches(const QDomNode& n, const NameTestWithPredicate<A,NotPredicate<Predicate>>& t) {
    bool m = node_filter_matches(n, t.tag);
    bool cm = true;
    if (m) {
        const auto atts = n.attributes();
        const int l = atts.length();
        int i = 0;
        while (cm && i < l) {
            auto a = atts.item(i);
            cm = node_filter_matches(a, t.predicate.predicate);
            ++i;
        }
        QDomNode c = n.firstChild();
        while (cm && !c.isNull()) {
            cm = node_filter_matches(c, t.predicate.predicate);
            c = c.nextSibling();
        }
    }
    return cm;
}

template <typename A, typename B>
bool
node_filter_matches(const QDomNode& n, const std::pair<A,B>& p) {
    return node_filter_matches(n, p.first) || node_filter_matches(n, p.second);
}

template <typename A, typename B>
bool
node_filter_matches(const QDomNode& n, const NameTestWithPredicate<A,B>& t) {
    bool m = node_filter_matches(n, t.tag);
    bool cm = false;
    if (m) {
        const auto atts = n.attributes();
        const int l = atts.length();
        int i = 0;
        while (!cm && i < l) {
            auto a = atts.item(i);
            cm = node_filter_matches(a, t.predicate);
            ++i;
        }
        QDomNode c = n.firstChild();
        while (!cm && !c.isNull()) {
            cm = node_filter_matches(c, t.predicate);
            c = c.nextSibling();
        }
    }
    return cm;
}

template <typename Filter>
std::list<QDomNode>
operator/(const QDomNode& node, const Filter& filter) {
    std::list<QDomNode> list;
    QDomNode n = node.firstChild();
    while (!n.isNull()) {
        if (node_filter_matches(n, filter)) {
            list.push_back(n);
        }
        n = n.nextSibling();
    }
    return list;
}

template <typename Filter>
std::list<QDomNode>
operator/(const std::list<QDomNode>& nodes, const Filter& filter) {
    std::list<QDomNode> list;
    for (const auto& l: nodes) {
        auto i = l/filter;
        for (const auto& n: i) {
            list.push_back(n);
        }
    }
    return list;
}

// % is used instead of the xpath expression //
// // means self-or-descendant
template <typename Filter>
std::list<QDomNode>
operator%(const QDomNode& node, const Filter& filter) {
    std::list<QDomNode> list;
    // check self::
    if (node_filter_matches(node, filter)) {
        list.push_back(node);
    }
    // recurse
    QDomNode n = node.firstChild();
    while (!n.isNull()) {
        for (const auto& cn: n%filter) {
            list.push_back(cn);
        }
        n = n.nextSibling();
    }
    return list;
}

template <typename Filter>
std::list<QDomNode>
operator%(const std::list<QDomNode>& nodes, const Filter& filter) {
    std::list<QDomNode> list;
    for (const auto& l: nodes) {
        auto i = l%filter;
        for (const auto& n: i) {
            list.push_back(n);
        }
    }
    return list;
}

#endif
