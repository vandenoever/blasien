#include <QObject>
#include <QTest>

class TestXPath : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
};

namespace {
}

QTEST_APPLESS_MAIN(TestXPath)
#include "testxpath.moc"
