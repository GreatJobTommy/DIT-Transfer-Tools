#include <QtTest/QtTest>
#include "../src/SpeedHistory.h"

class TestAvgs : public QObject {
    Q_OBJECT

private slots:
    void testAverageSpeed();
};

void TestAvgs::testAverageSpeed() {
    SpeedHistory history;
    history.addSpeed(1.0);
    history.addSpeed(2.0);
    history.addSpeed(3.0);

    qreal avg = history.averageSpeed();
    QCOMPARE(avg, 2.0);
}

QTEST_MAIN(TestAvgs)
#include "test_avgs.moc"