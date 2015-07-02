#include <QObject>
#include <QTest>
#include <XmlPath.h>
#include <XmlBuilder.h>
#include <XHtml11.h>

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

using namespace xhtml11;

namespace {
const QString idTag = QStringLiteral("id");
const QString empty;
XmlTag<QString,&empty, &idTag> id;

QDomDocument getExampleDoc1() {
    QDomDocument dom("test");
    XmlBuilder<XHtmlDocument>(dom)
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
        <xhtml11::div
          <p
            <"Second paragraph"
          >p
          <p
            <"Third paragraph"
          >p
        >xhtml11::div
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
