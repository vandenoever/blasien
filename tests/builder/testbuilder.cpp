#include <QObject>
#include <QTest>
#include <XmlBuilder.h>
#include <LiteralXml.h>

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

namespace {
const QString htmlns = QStringLiteral("http://www.w3.org/1999/xhtml");
const QString htmlTag = QStringLiteral("html");
const QString headTag = QStringLiteral("head");
const QString titleTag = QStringLiteral("title");
const QString idTag = QStringLiteral("id");
const QString classTag = QStringLiteral("class");
const QString empty;
XmlTag<&htmlns, &htmlTag, false, true> html;
XmlTag<&htmlns, &headTag, false, true> head;
XmlTag<&htmlns, &titleTag, false, true> title;
XmlTag<&empty, &idTag, true, false> id;
XmlTag<&empty, &classTag, true, false> class_;
}


void
TestBuilder::buildElement() {
    QDomDocument dom("test");
    XmlBuilder(dom)
    <html>html;
    QCOMPARE(dom.childNodes().length(), 1);
    QDomNode n = dom.firstChild();
    QCOMPARE(n.namespaceURI(), html.ns());
    QCOMPARE(n.localName(), html.name());
}
void
TestBuilder::buildElements() {
    QDomDocument dom("test");
    XmlBuilder(dom)
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
    XmlBuilder(dom)
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
    XmlBuilder(dom)
    <html
      <head({id="v1.1"})
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
    XmlBuilder(dom)
    <html
      <head({id="v1.1",class_="main"})
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
template <typename Base, typename Tag>
XmlSink<Base,Tag>
makeHead(const XmlSink<Base,Tag>& w) {
    return w
    <head>head;
}
void
TestBuilder::buildWithFunction() {
    QDomDocument dom("test");
    XmlBuilder(dom)
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
class Functor {
public:
    const QString text;
    Functor(const QString& text_) :text(text_) {}
    template <typename Base, typename Tag>
    XmlSink<Base,Tag> operator()(XmlSink<Base,Tag> w) {
        return w <head<text>head;
    }
};
void
TestBuilder::buildWithFunctor() {
    Functor f("HELLO");
    QDomDocument dom("test");
    XmlBuilder(dom)
    <html
      <f
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
    QDomNode t = n.firstChild();
    QCOMPARE(t.nodeValue(), QString("HELLO"));
}
class ListFunctor {
public:
    const std::list<QString> texts;
    ListFunctor(const std::list<QString>& texts_) :texts(texts_) {}
    template <typename Base, typename Tag>
    XmlSink<Base,Tag> operator()(const XmlSink<Base,Tag>& w) {
        for (const QString& t: texts) {
            w <head <t >head;
        }
        return w;
    }
};
void
TestBuilder::buildListWithFunctor() {
    ListFunctor f({"A","B"});
    QDomDocument dom("test");
    XmlBuilder(dom)
    <html
      <f
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
    QCOMPARE(t.nodeValue(), QString("A"));
    n = n.nextSibling();
    QCOMPARE(n.namespaceURI(), head.ns());
    QCOMPARE(n.localName(), head.name());
    QCOMPARE(n.childNodes().length(), 1);
    t = n.firstChild();
    QCOMPARE(t.nodeValue(), QString("B"));
}

QTEST_APPLESS_MAIN(TestBuilder)
#include "testbuilder.moc"
