#include <QObject>
#include <QTest>
#include <XmlPath.h>
#include <XmlBuilder.h>

class TestXPath : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
    void iterateWithTag();
    void iterateWithTags();
    void iterateWithOr();
    void iterateSelfWithDeepTags();
    void iterateWithDeepTags();
    void iterateWithTwoDeepTags();
    void iterateWithChildTag();
    void iterateWithMissingChildTag();
    void iterateWithChildTags();
    void iterateWithAttributePresence();
    void iterateWithAttributeNotPresence();
};

namespace {
const QString htmlns = QStringLiteral("http://www.w3.org/1999/xhtml");
const QString htmlTag = QStringLiteral("html");
const QString headTag = QStringLiteral("head");
const QString bodyTag = QStringLiteral("body");
const QString divTag = QStringLiteral("div");
const QString pTag = QStringLiteral("p");
const QString titleTag = QStringLiteral("title");
const QString idTag = QStringLiteral("id");
const QString classTag = QStringLiteral("class");
const QString empty;
XmlTag<&htmlns, &htmlTag, false, true> html;
XmlTag<&htmlns, &headTag, false, true> head;
XmlTag<&htmlns, &bodyTag, false, true> body;
XmlTag<&htmlns, &divTag, false, true> div;
XmlTag<&htmlns, &pTag, false, true> p;
XmlTag<&htmlns, &titleTag, false, true> title;
XmlTag<&empty, &idTag, true, false> id;
//XmlTag<&empty, &classTag, true, false> class_;

QDomDocument getExampleDoc1() {
    QDomDocument dom("test");
    XmlBuilder(dom)
    <html({id="html"})
      <head
        <title
          <"Hello World!"
        >title
      >head
      <body
        <p
          <"First paragraph"
        >p
        <div
          <p
            <"Second paragraph"
          >p
          <p
            <"Third paragraph"
          >p
        >div
      >body
    >html;
    return dom;
}
}

void
TestXPath::iterateWithTag() {
    QDomDocument dom = getExampleDoc1();
    int count = 0;
    for (auto node: dom/html) {
         ++count;
    }
    QCOMPARE(count, 1);
}

void
TestXPath::iterateWithTags() {
    QDomDocument dom = getExampleDoc1();
    int count = 0;
    for (auto node: dom/html/head) {
         ++count;
    }
    QCOMPARE(count, 1);
}

void
TestXPath::iterateWithOr() {
    QDomDocument dom = getExampleDoc1();
    int count = 0;
    for (auto node: dom/html/(body|head)) {
        ++count;
    }
    QCOMPARE(count, 2);
}

void
TestXPath::iterateSelfWithDeepTags() {
    QDomDocument dom = getExampleDoc1();
    QDomElement h = dom.firstChildElement();
    int count = 0;
    for (auto node: h%html) {
        ++count;
    }
    QCOMPARE(count, 1);
}

void
TestXPath::iterateWithDeepTags() {
    QDomDocument dom = getExampleDoc1();
    int count = 0;
    for (auto node: dom%p) {
        ++count;
    }
    QCOMPARE(count, 3);
}

void
TestXPath::iterateWithTwoDeepTags() {
    QDomDocument dom = getExampleDoc1();
    int count = 0;
    for (auto node: dom%html%p) {
        ++count;
    }
    QCOMPARE(count, 3);
}
void
TestXPath::iterateWithChildTag() {
    QDomDocument dom = getExampleDoc1();
    int count = 0;
    for (auto node: dom/html[body]) {
        ++count;
    }
    QCOMPARE(count, 1);
}
void
TestXPath::iterateWithMissingChildTag() {
    QDomDocument dom = getExampleDoc1();
    int count = 0;
    for (auto node: dom/html[p]) {
        ++count;
    }
    QCOMPARE(count, 0);
}
void
TestXPath::iterateWithChildTags() {
    QDomDocument dom = getExampleDoc1();
    int count = 0;
    for (auto node: dom/html[p|body]) {
        ++count;
    }
    QCOMPARE(count, 1);
}
void
TestXPath::iterateWithAttributePresence() {
    QDomDocument dom = getExampleDoc1();
    int count = 0;
    for (auto node: dom/html[~id]) {
        ++count;
    }
    QCOMPARE(count, 1);
}
void
TestXPath::iterateWithAttributeNotPresence() {
    QDomDocument dom = getExampleDoc1();
    int count = 0;
    for (auto node: dom/html[not(~id)]) {
        ++count;
    }
    QCOMPARE(count, 0);
}

QTEST_APPLESS_MAIN(TestXPath)
#include "testxpath.moc"
