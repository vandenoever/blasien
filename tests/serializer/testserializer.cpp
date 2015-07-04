#include <QObject>
#include <QTest>
#include <XmlWriter.h>
#include <LiteralXml.h>
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
    void writeElementWithRequiredAttribute();
};

namespace {
xhtml11::IdTag id;
xhtml11::ClassTag class_;
}

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
template <typename Base, typename Tag>
XmlSink<Base,Tag>
makeHead(const XmlSink<Base,Tag>& w) {
    return w
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

class Functor {
public:
    const QString text;
    Functor(const QString& text_) :text(text_) {}
    template <typename Base, typename Tag>
    XmlSink<Base,Tag> operator()(const XmlSink<Base,Tag>& w) {
        return w <head<title<text>title>head;
    }
};
void
TestSerializer::writeWithFunctor() {
    Functor f("HELLO");
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<xhtml11::XHtmlDocument>(stream)
    <html
      <f
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head><n1:title>HELLO</n1:title></n1:head></n1:html>"));
}
class ListFunctor {
public:
    const std::list<QString> texts;
    ListFunctor(const std::list<QString>& texts_) :texts(texts_) {}
    template <typename Base, typename Tag>
    XmlSink<Base,Tag> operator()(XmlSink<Base,Tag> w) {
        for (const QString& t: texts) {
            w <head<title<t>title>head;
        }
        return w;
    }
};
void
TestSerializer::writeListWithFunctor() {
    ListFunctor f({"A","B"});
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<xhtml11::XHtmlDocument>(stream)
    <html
      <f
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head><n1:title>A</n1:title></n1:head><n1:head><n1:title>B</n1:title></n1:head></n1:html>"));
}
void
TestSerializer::writeElementWithRequiredAttribute() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<xhtml11::XHtmlDocument>(stream)
    <html
      <body
//        <img
//        >img
      >body
    >html;

}

QTEST_APPLESS_MAIN(TestSerializer)
#include "testserializer.moc"
