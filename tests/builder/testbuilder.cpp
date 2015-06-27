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
    Q_ASSERT(!dom.isNull());
    XmlBuilder<>(dom)
    <html>html;
    QCOMPARE(dom.childNodes().length(), 1);
    QDomNode n = dom.firstChild();
    QCOMPARE(n.namespaceURI(), html.ns());
    QCOMPARE(n.localName(), html.name());
}

void
TestBuilder::buildElements() {
}
void
TestBuilder::buildCharacters() {}
void
TestBuilder::buildAttribute() {
}
void
TestBuilder::buildAttributes() {
}
template <typename Base, typename Tag>
XmlBuilder<Base,Tag>
makeHead(XmlBuilder<Base,Tag> w) {
    return w
    <head>head;
}
void
TestBuilder::buildWithFunction() {
}
class Functor {
public:
    const QString text;
    Functor(const QString& text_) :text(text_) {}
    template <typename Base, typename Tag>
    XmlBuilder<Base,Tag> operator()(XmlBuilder<Base,Tag> w) {
        return w <head<text>head;
    }
};
void
TestBuilder::buildWithFunctor() {
}
class ListFunctor {
public:
    const std::list<QString> texts;
    ListFunctor(const std::list<QString>& texts_) :texts(texts_) {}
    template <typename Base, typename Tag>
    XmlBuilder<Base,Tag> operator()(XmlBuilder<Base,Tag> w) {
        for (const QString& t: texts) {
            w <head <t >head;
        }
        return w;
    }
};
void
TestBuilder::buildListWithFunctor() {
}


QTEST_APPLESS_MAIN(TestBuilder)
#include "testbuilder.moc"
