#include <QObject>
#include <QTest>
#include <XmlBuilder.h>
#include <XHtml11.h>

class TestBuilder : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
    void buildElement();
    void buildElements();
    void buildCharacters();
    void buildAttribute();
    void buildAttributes();
    void buildWithFunction();
    void buildWithFunctor();
    void buildListWithFunctor();
};

using namespace xhtml11;

void
TestBuilder::buildElement() {
    QDomDocument dom("test");
    XmlBuilder<XHtmlDocument>(dom)
    <html>html;
    QCOMPARE(dom.childNodes().length(), 1);
    QDomNode n = dom.firstChild();
    QCOMPARE(n.namespaceURI(), html.ns());
    QCOMPARE(n.localName(), html.name());
}
void
TestBuilder::buildElements() {
    QDomDocument dom("test");
    XmlBuilder<XHtmlDocument>(dom)
    <html
      <head>head
    >html;
    QCOMPARE(dom.childNodes().length(), 1);
    QDomNode n = dom.firstChild();
    QCOMPARE(n.namespaceURI(), html.ns());
    QCOMPARE(n.localName(), html.name());
    QCOMPARE(n.childNodes().length(), 1);
    n = n.firstChild();
    QCOMPARE(n.namespaceURI(), head.ns());
    QCOMPARE(n.localName(), head.name());
}
void
TestBuilder::buildCharacters() {
    QDomDocument dom("test");
    XmlBuilder<XHtmlDocument>(dom)
    <html
      <head
        <title
          <"Hello world"
        >title
      >head
    >html;
    QCOMPARE(dom.childNodes().length(), 1);
    QDomNode n = dom.firstChild();
    QCOMPARE(n.namespaceURI(), html.ns());
    QCOMPARE(n.localName(), html.name());
    QCOMPARE(n.childNodes().length(), 1);
    n = n.firstChild();
    QCOMPARE(n.namespaceURI(), head.ns());
    QCOMPARE(n.localName(), head.name());
    QCOMPARE(n.childNodes().length(), 1);
    n = n.firstChild();
    QCOMPARE(n.namespaceURI(), title.ns());
    QCOMPARE(n.localName(), title.name());
    QCOMPARE(n.childNodes().length(), 1);
}
void
TestBuilder::buildAttribute() {
    QDomDocument dom("test");
    XmlBuilder<XHtmlDocument>(dom)
    <html
      <head(id="v1.1")
      >head
    >html;
    QCOMPARE(dom.childNodes().length(), 1);
    QDomNode n = dom.firstChild();
    QCOMPARE(n.namespaceURI(), html.ns());
    QCOMPARE(n.localName(), html.name());
    QCOMPARE(n.childNodes().length(), 1);
    n = n.firstChild();
    QCOMPARE(n.namespaceURI(), head.ns());
    QCOMPARE(n.localName(), head.name());
    QCOMPARE(n.attributes().count(), 1);
    QDomNode a = n.attributes().item(0);
    QCOMPARE(a.namespaceURI(), id.ns());
    QCOMPARE(a.localName(), id.name());
    QCOMPARE(a.nodeValue(), QString("v1.1"));
}
void
TestBuilder::buildAttributes() {
    QDomDocument dom("test");
    XmlBuilder<XHtmlDocument>(dom)
    <html
      <head(id="v1.1",class_="main")
      >head
    >html;
    QCOMPARE(dom.childNodes().length(), 1);
    QDomNode n = dom.firstChild();
    QCOMPARE(n.namespaceURI(), html.ns());
    QCOMPARE(n.localName(), html.name());
    QCOMPARE(n.childNodes().length(), 1);
    n = n.firstChild();
    QCOMPARE(n.namespaceURI(), head.ns());
    QCOMPARE(n.localName(), head.name());
    QCOMPARE(n.attributes().count(), 2);
    QDomNode a = n.attributes().namedItem(id.name());
    QCOMPARE(a.nodeValue(), QString("v1.1"));
    a = n.attributes().namedItem(class_.name());
    QCOMPARE(a.nodeValue(), QString("main"));
}
template <typename Sink>
Sink
makeHead(const Sink& sink) {
    return sink
    <head>head;
}
void
TestBuilder::buildWithFunction() {
    QDomDocument dom("test");
    XmlBuilder<XHtmlDocument>(dom)
    <html
      <makeHead
    >html;
    QCOMPARE(dom.childNodes().length(), 1);
    QDomNode n = dom.firstChild();
    QCOMPARE(n.namespaceURI(), html.ns());
    QCOMPARE(n.localName(), html.name());
    QCOMPARE(n.childNodes().length(), 1);
    n = n.firstChild();
    QCOMPARE(n.namespaceURI(), head.ns());
    QCOMPARE(n.localName(), head.name());
}
struct Functor {
    const QString text;
    template <typename Sink>
    Sink operator()(const Sink& sink) {
        return sink <head<title<text>title>head;
    }
};
void
TestBuilder::buildWithFunctor() {
    QDomDocument dom("test");
    XmlBuilder<XHtmlDocument>(dom)
    <html
      <Functor{"HELLO"}
    >html;
    QCOMPARE(dom.childNodes().length(), 1);
    QDomNode n = dom.firstChild();
    QCOMPARE(n.namespaceURI(), html.ns());
    QCOMPARE(n.localName(), html.name());
    QCOMPARE(n.childNodes().length(), 1);
    n = n.firstChild();
    QCOMPARE(n.namespaceURI(), head.ns());
    QCOMPARE(n.localName(), head.name());
    QCOMPARE(n.childNodes().length(), 1);
    n = n.firstChild();
    QCOMPARE(n.namespaceURI(), title.ns());
    QCOMPARE(n.localName(), title.name());
    QCOMPARE(n.childNodes().length(), 1);
    QDomNode t = n.firstChild();
    QCOMPARE(t.nodeValue(), QString("HELLO"));
}
struct ListFunctor {
    const std::list<QString> texts;
    template <typename Sink>
    Sink operator()(const Sink& sink) {
        for (const QString& t: texts) {
            sink <head<title<t>title>head;
        }
        return sink;
    }
};
void
TestBuilder::buildListWithFunctor() {
    QDomDocument dom("test");
    XmlBuilder<XHtmlDocument>(dom)
    <html
      <ListFunctor{{"A","B"}}
    >html;
    QCOMPARE(dom.childNodes().length(), 1);
    QDomNode n = dom.firstChild();
    QCOMPARE(n.namespaceURI(), html.ns());
    QCOMPARE(n.localName(), html.name());
    QCOMPARE(n.childNodes().length(), 2);
    n = n.firstChild();
    QCOMPARE(n.namespaceURI(), head.ns());
    QCOMPARE(n.localName(), head.name());
    QCOMPARE(n.childNodes().length(), 1);
    QDomNode t = n.firstChild();
    QCOMPARE(t.namespaceURI(), title.ns());
    QCOMPARE(t.localName(), title.name());
    QCOMPARE(t.childNodes().length(), 1);
    t = t.firstChild();
    QCOMPARE(t.nodeValue(), QString("A"));
    n = n.nextSibling();
    QCOMPARE(n.namespaceURI(), head.ns());
    QCOMPARE(n.localName(), head.name());
    QCOMPARE(n.childNodes().length(), 1);
    t = n.firstChild();
    QCOMPARE(t.namespaceURI(), title.ns());
    QCOMPARE(t.localName(), title.name());
    QCOMPARE(t.childNodes().length(), 1);
    t = t.firstChild();
    QCOMPARE(t.nodeValue(), QString("B"));
}

QTEST_APPLESS_MAIN(TestBuilder)
#include "testbuilder.moc"
