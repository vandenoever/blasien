#include <QCoreApplication>

/**
  XPath is a convenient way to iterator througn a XML tree.
  With XPath you walk through nodes.
 **/

class NodeIterator {
public:
};

class QName;
class NodeFilter {
public:
    NodeFilter() {}
    NodeFilter(const QName&) {}
    NodeFilter operator[](int) const {
        return NodeFilter();
    }
    NodeFilter operator[](const QName&) const {
        return NodeFilter();
    }
};

struct AttributeName;
struct QName {
    const QString ns;
    const QString name;
    QName(const QString&, const QString&) {}
    NodeFilter operator[](int) const {
        return NodeFilter();
    }
    NodeFilter operator[](const AttributeName&) const {
        return NodeFilter();
    }
    NodeFilter operator[](const NodeFilter&) const {
        return NodeFilter();
    }
};

struct AttributeName {
    const QString ns;
    const QString name;
};

QString xhtmlns = "http://www.w3.org/1999/xhtml";

namespace h {
    static const QName html { xhtmlns, "html" };
    static const QName head { xhtmlns, "head" };
    static const QName title { xhtmlns, "title" };
    static const QName body { xhtmlns, "body" };
    static const QName div { xhtmlns, "div" };
    static const QName span { xhtmlns, "span" };
    static const QName p { xhtmlns, "p" };
    static const QName id { xhtmlns, "id" };
}
using namespace h;

NodeFilter count(NodeFilter) {
    return NodeFilter();
}

NodeFilter operator==(NodeFilter, int) {
    return NodeFilter();
}

NodeIterator operator/(NodeIterator, QName) {
    return NodeIterator();
}

NodeIterator operator/(NodeIterator, NodeFilter) {
    return NodeIterator();
}

NodeIterator operator/=(NodeIterator, QName) {
    return NodeIterator();
}

NodeIterator operator/=(NodeIterator, NodeFilter) {
    return NodeIterator();
}

AttributeName operator~(QName) {
    return AttributeName();
}

void xpath() {
    auto doc = NodeIterator();
    doc/html/body/p[1][span];
    doc/=p[count(span) == 4];
    doc/=p[~id];
    auto d = { html, body };
}

class a {
public:
    a(QName, QString) {}
};

class AttributeIterator {
public:
    AttributeIterator() {}
    AttributeIterator(a) {}
};

class Child {
public:
    Child(QName) {}
    Child(NodeIterator) {}
    Child(AttributeIterator) {}
    Child(QString) {}
    Child(const char*) {}
};

class e {
public:
    e(std::initializer_list<Child>) {}
};

NodeIterator operator+(QName, e) {
    return NodeIterator();
}

NodeIterator operator+(QName, QString) {
    return NodeIterator();
}

AttributeIterator operator+(QName, a) {
    return AttributeIterator();
}


AttributeIterator operator+(AttributeIterator, a) {
    return AttributeIterator();
}

void write() {
    html + e{
        head + e{
            title + "Title"
        },
        body + e{
            h::div + a(id, "yo") + a(id, "yo"),
            h::div + "Hello World",
            h::div + e{"Hello ", span + "World"}
        }
    };
    e{h::div};
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}
