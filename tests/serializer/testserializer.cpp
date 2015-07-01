#include <QObject>
#include <QTest>
#include <XmlWriter.h>
#include <LiteralXml.h>
#include <QDebug>

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

struct DivType {
    constexpr DivType() {}
};

struct PType {
    constexpr PType() {}
};

template <typename ElementType>
constexpr std::tuple<> getAllowedChildElements(const ElementType&) {
    return std::tuple<>();
}

constexpr std::tuple<DivType, PType> getAllowedChildElements(const DivType&) {
    return std::tuple<DivType, PType>();
}
constexpr std::tuple<PType> getAllowedChildElements(const PType&) {
    return std::tuple<PType>();
}


template <typename T, typename Tuple>
struct tuple_contains_type;

template <typename T>
struct tuple_contains_type<T, std::tuple<>> : std::false_type {};

template <typename T, typename U, typename... Ts>
struct tuple_contains_type<T, std::tuple<U, Ts...>> : tuple_contains_type<T, std::tuple<Ts...>> {};

template <typename T, typename... Ts>
struct tuple_contains_type<T, std::tuple<T, Ts...>> : std::true_type {};

template <typename ParentElementType, typename ChildElementType>
constexpr bool
isChildAllowed() {
    using Tuple = decltype(getAllowedChildElements(ParentElementType()));
    return tuple_contains_type<ChildElementType,Tuple>();
}

void test() {
    auto a = getAllowedChildElements(DivType());
    using T = decltype(a);
    qDebug() << std::tuple_size<T>::value;
    qDebug() << isChildAllowed<DivType, PType>();
    qDebug() << isChildAllowed<PType, DivType>();
}

}

void
TestSerializer::writeElement() {
    test();
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter(stream)
    <html>html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"/>"));
}
void
TestSerializer::writeElements() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter(stream)
    <html
      <head>head
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head/></n1:html>"));
}
void
TestSerializer::writeCharacters() {
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter(stream)
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
    XmlWriter(stream)
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
    XmlWriter(stream)
    <html
      <head({id="v1.1",class_="main"})
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
    XmlWriter(stream)
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
        return w <head<text>head;
    }
};
void
TestSerializer::writeWithFunctor() {
    Functor f("HELLO");
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter(stream)
    <html
      <f
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head>HELLO</n1:head></n1:html>"));
}
class ListFunctor {
public:
    const std::list<QString> texts;
    ListFunctor(const std::list<QString>& texts_) :texts(texts_) {}
    template <typename Base, typename Tag>
    XmlSink<Base,Tag> operator()(XmlSink<Base,Tag> w) {
        for (const QString& t: texts) {
            w <head <t >head;
        }
        return w;
    }
};
void
TestSerializer::writeListWithFunctor() {
    ListFunctor f({"A","B"});
    QString r;
    QXmlStreamWriter stream(&r);
    XmlWriter(stream)
    <html
      <f
    >html;
    QCOMPARE(r, QString("<n1:html xmlns:n1=\"http://www.w3.org/1999/xhtml\"><n1:head>A</n1:head><n1:head>B</n1:head></n1:html>"));
}

QTEST_APPLESS_MAIN(TestSerializer)
#include "testserializer.moc"
