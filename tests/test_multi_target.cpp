#include <QtTest/QtTest>
#include <QMap>
#include "TransferTask.h"

class TestMultiTarget : public QObject {
    Q_OBJECT

private slots:
    void testMultiDestinationTask() {
        QMap<QString, QString> dests;
        dests["dest1"] = "/tmp/dest1";
        dests["dest2"] = "/tmp/dest2";
        TransferTask task("/tmp/src", dests);

        QCOMPARE(task.source(), QString("/tmp/src"));
        QCOMPARE(task.destinations(), dests);
        QVERIFY(task.destinations().size() == 2);
    }

    void testProgressPerDest() {
        QMap<QString, QString> dests;
        dests["dest1"] = "/tmp/dest1";
        TransferTask task("/tmp/src", dests);

        // Simulate run
        task.run();

        QVERIFY(task.isFinished());
        QVERIFY(task.success());
        QVERIFY(task.bytesTransferredPerDest().contains("/tmp/dest1"));
    }
};

QTEST_MAIN(TestMultiTarget)
#include "test_multi_target.moc"