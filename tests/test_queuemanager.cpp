#include &lt;QTest&gt;
#include &quot;../src/QueueManager.h&quot;
#include &quot;../src/TransferTask.h&quot;
#include &lt;QSignalSpy&gt;

class TestQueueManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        m_manager = new QueueManager();
    }

    void cleanupTestCase() {
        delete m_manager;
    }

    void testAddTask() {
        TransferTask *task = new TransferTask();
        m_manager-&gt;addTask(task);
        QCOMPARE(m_manager-&gt;getTasks().size(), 1);
        QCOMPARE(task-&gt;status(), TaskStatus::Pending);
    }

    void testMaxParallel() {
        m_manager-&gt;setMaxParallel(2);
        QCOMPARE(m_manager-&gt;getMaxParallel(), 2);
    }

    void testMoveTask() {
        TransferTask *t1 = new TransferTask();
        TransferTask *t2 = new TransferTask();
        m_manager-&gt;addTask(t1);
        m_manager-&gt;addTask(t2);
        m_manager-&gt;moveTask(1, 0);
        // Check reorder via signal or list
        QSignalSpy spy(m_manager, &QueueManager::tasksReordered);
        QCOMPARE(spy.count(), 1);
    }

private:
    QueueManager *m_manager;
};

QTEST_MAIN(TestQueueManager)
#include &quot;test_queuemanager.moc&quot;
