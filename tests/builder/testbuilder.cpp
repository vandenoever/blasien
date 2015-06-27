#include <QObject>
#include <QTest>

class TestBuilder : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
};

namespace {
}

QTEST_APPLESS_MAIN(TestBuilder)
#include "testbuilder.moc"
