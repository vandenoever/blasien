#include <QObject>
#include <QTest>
#include <XmlWriter.h>
#include <XHtml11.h>

class TestSerializer : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
    void writeElement();
    void writeElements();
    void writeCharacters();
    void writeAttribute();
    void writeAttributes();
    void writeWithFunction();
    void writeWithFunctor();
    void writeListWithFunctor();
    void writeElementWithRequiredAttributes();
    void writeLoop();
};

using namespace xhtml11;

void
TestSerializer::writeElement() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<XHtmlDocument>(stream)
    <html>html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"/>"));
}
void
TestSerializer::writeElements() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<xhtml11::XHtmlDocument>(stream)
    <html
      <head>head
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head/></n1:html>"));
}
void
TestSerializer::writeCharacters() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<xhtml11::XHtmlDocument>(stream)
    <html
      <head
        <title
          <"Hello world"
        >title
      >head
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head><n1:title>Hello world</n1:title></n1:head></n1:html>"));
}
void
TestSerializer::writeAttribute() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<xhtml11::XHtmlDocument>(stream)
    <html
      <head(id="v1.1")
      >head
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head id=\"v1.1\"/></n1:html>"));
}
void
TestSerializer::writeAttributes() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<xhtml11::XHtmlDocument>(stream)
    <html
      <head(id="v1.1",class_="main")
      >head
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head id=\"v1.1\" class=\"main\"/></n1:html>"));
}
template <typename Sink>
Sink
makeHead(const Sink& sink) {
    return sink
    <head>head;
}
void
TestSerializer::writeWithFunction() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<xhtml11::XHtmlDocument>(stream)
    <html
      <makeHead
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head/></n1:html>"));
}

struct Functor {
    const QString text;
    template <typename Sink>
    Sink operator()(const Sink& sink) {
        return sink <head<title<text>title>head;
    }
};
void
TestSerializer::writeWithFunctor() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<xhtml11::XHtmlDocument>(stream)
    <html
      <Functor{{"HELLO"}}
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head><n1:title>HELLO</n1:title></n1:head></n1:html>"));
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
TestSerializer::writeListWithFunctor() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<xhtml11::XHtmlDocument>(stream)
    <html
      <ListFunctor{{"A","B"}}
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head><n1:title>A</n1:title></n1:head><n1:head><n1:title>B</n1:title></n1:head></n1:html>"));
}
void
TestSerializer::writeElementWithRequiredAttributes() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<xhtml11::XHtmlDocument>(stream)
    <html
      <body
        <img(src="urn:ok",alt="Hello")
        >img
      >body
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:body><n1:img src=\"urn:ok\" alt=\"Hello\"/></n1:body></n1:html>"));
}
void
TestSerializer::writeLoop() {
    QString r;
    QXmlStreamWriter stream(&r);
    auto items = std::list<QString>({"A","B"});
    XmlWriter<xhtml11::XHtmlDocument> xml(stream);
    // in c++14 one could use 'auto' in the lambda instead of deriving the Sink type
    using Sink = decltype(xml<html<body);

    xml
    <html
        <body
            <for_each(items, [](Sink sink, const QString& text){return sink
                <p
                    <text
                >p;
            })
        >body
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:body><n1:p>A</n1:p><n1:p>B</n1:p></n1:body></n1:html>"));
}

QTEST_APPLESS_MAIN(TestSerializer)
#include "testserializer.moc"
