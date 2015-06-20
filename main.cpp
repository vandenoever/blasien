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
    NodeFilter operator[](int i) {
        return NodeFilter();
    }
    NodeFilter operator[](const QName& i) {
        return NodeFilter();
    }
};

struct AttributeName;
struct QName {
    QString ns;
    QString name;
    QName(QString, QString) {}
    QName(const NodeFilter&) {}
    NodeFilter operator[](int i) {
        return NodeFilter();
    }
    NodeFilter operator[](const AttributeName&) {

    }
};

struct AttributeName {
    QString ns;
    QString name;
};

QString xhtmlns = "http://www.w3.org/1999/xhtml";

namespace h {
    static QName html { xhtmlns, "html" };
    static QName head { xhtmlns, "head" };
    static QName title { xhtmlns, "title" };
    static QName body { xhtmlns, "body" };
    static QName div { xhtmlns, "div" };
    static QName span { xhtmlns, "span" };
    static QName p { xhtmlns, "p" };
    static QName id { xhtmlns, "id" };
}
using namespace h;

NodeFilter count(const NodeFilter& f) {
    return NodeFilter();
}

NodeFilter operator==(NodeFilter, int) {

}

NodeIterator operator/(NodeIterator p, QName) {
    return NodeIterator();
}

NodeIterator operator/(NodeIterator p, NodeFilter) {
    return NodeIterator();
}

NodeIterator operator||(NodeIterator p, QName) {
    return NodeIterator();
}

AttributeName operator~(QName) {
    return AttributeName();
}

void test1() {
    auto doc = NodeIterator();
    doc/html/body/p[1][span];//[count(span) == 4];
    doc||p[~id];
    auto d = { html, body };
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}
