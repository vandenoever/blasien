#include <QObject>
#include <QTest>
#include <XmlWriter.h>
#include <LiteralXml.h>
class TestSerializer : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
    void writeElement();
    void writeElements();
    void writeCharacters();
    void writeAttribute();
    void writeAttributes();
    void writeWithFunctor();
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
TestSerializer::writeElement() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<>(stream)
    <html>html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"/>"));
}

void
TestSerializer::writeElements() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<>(stream)
    <html
      <head>head
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head/></n1:html>"));
}
void
TestSerializer::writeCharacters() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<>(stream)
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
    XmlWriter<>(stream)
    <html
      <head({id="v1.1"})
      >head
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head id=\"v1.1\"/></n1:html>"));
}
void
TestSerializer::writeAttributes() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<>(stream)
    <html
      <head({id="v1.1",class_="main"})
      >head
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head id=\"v1.1\" class=\"main\"/></n1:html>"));
}
class Functor {
public:
    template <typename Base, typename Tag>
    XmlWriter<Base,Tag> operator()(XmlWriter<Base,Tag> w) {return w;}
};
void
TestSerializer::writeWithFunctor() {
    Functor f;
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<>(stream)
    <html
      <f
    >html;
}


QTEST_MAIN(TestSerializer)
#include "testserializer.moc"
