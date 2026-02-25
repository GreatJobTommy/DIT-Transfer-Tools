#include <QtTest>
#include "../src/QueueManager.h"
#include "../src/TransferTask.h"

class TestQueueManager : public QObject {
    Q_OBJECT

private slots:
    void testReorderWaiting();
    void testParallel10();
};

void TestQueueManager::testReorderWaiting() {
    QueueManager qm(1);
    TransferTask* t1 = new TransferTask("s1", "d1");
    TransferTask* t2 = new TransferTask("s2", "d2");
    TransferTask* t3 = new TransferTask("s3", "d3");
    qm.addTask(t1);
    qm.addTask(t2);
    qm.addTask(t3);

    // Initial waiting: t2, t3 (t1 active)
    QCOMPARE(qm.waitingTasks().size(), 2);
    QCOMPARE(qm.waitingTasks()[0]->source(), QString("s2"));
    QCOMPARE(qm.waitingTasks()[1]->source(), QString("s3"));

    qm.reorderWaiting(0, 1); // move t2 to position 1
    QCOMPARE(qm.waitingTasks()[0]->source(), QString("s3"));
    QCOMPARE(qm.waitingTasks()[1]->source(), QString("s2"));
}

void TestQueueManager::testParallel10() {
    QueueManager qm(10);
    for (int i = 0; i < 15; ++i) {
        TransferTask* t = new TransferTask(QString("s%1").arg(i), QString("d%1").arg(i));
        qm.addTask(t);
    }

    QCOMPARE(qm.activeTasks().size(), 10);
    QCOMPARE(qm.waitingTasks().size(), 5);
}

QTEST_APPLESS_MAIN(TestQueueManager)
#include "test_queuemanager.moc"