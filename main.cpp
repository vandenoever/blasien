#include <QCoreApplication>
#include <QXmlStreamWriter>
#include <QFile>
#include <algorithm>

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
struct AttributeOrElement;
struct AttributeNode;

struct QName {
    const QString ns;
    const QString name;
    QName(const QString& ns_, const QString& name_) :ns(ns_), name(name_) {}
    /** XPath **/
    NodeFilter operator[](int) const {
        return NodeFilter();
    }
    NodeFilter operator[](const AttributeName&) const {
        return NodeFilter();
    }
    NodeFilter operator[](const NodeFilter&) const {
        return NodeFilter();
    }
    /** Serialization **/
    AttributeOrElement operator()(const char*val) const;
    /** this should return a function that can be called with a writer **/
    template<typename... Args>
    NodeIterator operator()(const QName&, Args... args) const;
    /*
    NodeIterator operator()() const;
    NodeIterator operator()(std::initializer_list<AttributeNode>) const;
    template<typename... Args>
    NodeIterator operator()(const char*, Args... args) const;
    template<typename... Args>
    NodeIterator operator()(const NodeIterator&, Args... args) const;
    template<typename... Args>
    NodeIterator operator()(const AttributeOrElement&, Args... args) const;
    */
};

struct AttributeOrElement {
    const QName qname;
    const QString value;
    AttributeOrElement(const QName& q, const QString& v) :qname(q), value(v) {}
};
AttributeOrElement QName::operator()(const char*val) const {
    return AttributeOrElement(*this, val);
}

//struct AttributeName;
struct AttributeNode {
    const QName qname;
    const QString value;
    AttributeNode(const QName& q, const QString& v) :qname(q), value(v) {}
    AttributeNode(const AttributeOrElement& a) :qname(a.qname), value(a.value) {}
};
/*
class Child;
struct ElementHead {
    ElementHead(const QName&) {}
    template<typename... Args>
    NodeIterator operator()(const Child& t, Args... args) const {
        return NodeIterator();
    }
};
*/

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
    static const QName id { QString(), "id" };
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

class AttributeIterator {
public:
    AttributeIterator() {}
    AttributeIterator(AttributeNode) {}
};
/*
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

NodeIterator operator>(QName, e) {
    return NodeIterator();
}
*/

NodeIterator operator>(QName, NodeIterator) {
    return NodeIterator();
}

NodeIterator operator+(QName, QString) {
    return NodeIterator();
}

AttributeIterator operator+(QName, AttributeNode) {
    return AttributeIterator();
}

AttributeIterator operator+(AttributeIterator, AttributeNode) {
    return AttributeIterator();
}
/*
NodeIterator operator+(AttributeIterator, e) {
    return NodeIterator();
}
*/
struct ElementBodyWriter {
    QXmlStreamWriter& writer;
    ElementBodyWriter(QXmlStreamWriter& w) :writer(w) {
    }
};
struct ElementWriter {
    QXmlStreamWriter& writer;
    ElementWriter(QXmlStreamWriter& w, const QName& qname) :writer(w) {
        writer.writeStartElement(qname.ns, qname.name);
    }
    ~ElementWriter() {
        writer.writeEndElement();
    }
    void operator()() const {}
    template<typename... Args>
    void operator()(const QName& qname, Args... args) const {
        ElementWriter(writer, qname);
        (*this)(args...);
    }
    template<typename... Args>
    void operator()(const AttributeOrElement& aoe, Args... args) const {
        ElementWriter(writer, aoe.qname)(aoe.value);
        (*this)(args...);
    }
    template<typename... Args>
    void operator()(const QString& v, Args... args) const {
        writer.writeCharacters(v);
        (*this)(args...);
    }
    ElementBodyWriter operator()(std::initializer_list<AttributeNode> lst) const {
        for (auto att: lst) {
            writer.writeAttribute(att.qname.ns, att.qname.name, att.value);
        }
        return ElementBodyWriter(writer);
    }
};
struct XmlWriter {
    QXmlStreamWriter& writer;
    XmlWriter(QXmlStreamWriter& w) :writer(w) {}
    ElementWriter operator()(const QName& qname) {
        return ElementWriter(writer, qname);
    }
};

void write() {
    QFile file;
    file.open(stderr, QIODevice::WriteOnly);
    QXmlStreamWriter writer(&file);
    XmlWriter w(writer);

    w(html)();
    w(html)(head);
    w(html)({id("hi")});

    //html(head);

 //   w(html)(head(title("hello")));
/*
    html({id("hi")});
    html(head(title("hello")));
    html(
        head(
            title("Title")
        ),
        body(
            //h::div(id("yo"), id("yo"))(span),
            //h::div("Hello World"),
            h::div("Hello ", span("World"))
        )
    );*/
}

struct XmlWriter2 {
    QXmlStreamWriter& writer;
    XmlWriter2(QXmlStreamWriter& w) :writer(w) {}
    ElementWriter operator()(const QName& qname) {
        return ElementWriter(writer, qname);
    }
};
/*
void write2() {
    QFile file;
    file.open(stderr, QIODevice::WriteOnly);
    QXmlStreamWriter writer(&file);
    XmlWriter2 w(writer);
    w <<
    html()>
        head>
            title>
                "Title"
        <body>
            h::div>
                "Hello "
                <span>"World"
    ;
}

void write3() {
    QFile file;
    file.open(stderr, QIODevice::WriteOnly);
    QXmlStreamWriter writer(&file);
    XmlWriter w(stdout);
    w(html,
        w(head,
            w(title,
                "Title")
        ),
        w(body,
            w(h::div,
                "Hello ",
                w(span, "World")
            )
        )
    );
}*/


class TieTaTest {
public:
    void html() const {}
    void head() const {}
};

void lambda() {
    TieTaTest t;
    [t](){
        t.head();
    }();
    [&](){
        t.head();
    }();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    write();

    return a.exec();
}
