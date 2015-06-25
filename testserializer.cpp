#include <QObject>
#include <QTest>
#include <XmlWriter.h>
#include <LiteralXml.h>
class TestSerializer : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
    void writeElement();
    void writeElement_();
    void writeElements();
    void writeCharacters();
    void writeAttribute();
    void writeAttributes();
};

namespace {

template <const QString* Ns, const QString* Name>
struct XmlTag {
    static const bool is_tag = true;
    static const QName qname;
    static const QString& ns() {
        return qname.ns;
    }
    static const QString& name() {
        return qname.name;
    }
};

template <const QString* Ns, const QString* Name>
const QName XmlTag<Ns, Name>::qname(*Ns, *Name);

const QString htmlns = QStringLiteral("http://www.w3.org/1999/xhtml");
const QString htmlTag = QStringLiteral("html");
XmlTag<&htmlns, &htmlTag> html_;

static QString xhtmlns = "http://www.w3.org/1999/xhtml";

struct HtmlQName : public QName {
    HtmlQName() :QName(xhtmlns, "html") {}
};
struct HeadQName : public QName {
    HeadQName() :QName(xhtmlns, "head") {}
    ElementStart<HeadQName> operator()(std::initializer_list<AttributeNode> atts) const {
        return ElementStart<HeadQName>(*this, atts);
    }
};
struct TitleQName : public QName {
    TitleQName() :QName(xhtmlns, "title") {}
};
struct IdQName : public QName {
    IdQName() :QName(QString(), "id") {}
    AttributeNode operator=(const QString& val) const {
        return AttributeNode(*this, val);
    }
};

static const HtmlQName html;
static const HeadQName head;
static const TitleQName title;
static const IdQName id;

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
TestSerializer::writeElement_() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter<>(stream)
    <html_>html_;
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
}


QTEST_MAIN(TestSerializer)
#include "testserializer.moc"
