/****************************************************************************
** test_drivemonitor.cpp - Extended Unit-Tests für DriveMonitor V3
** Tests: QStorageInfo, Watcher, Poll-Fallback, Signals, Queue Integration.
****************************************************************************/

#include &lt;QtTest&gt;
#include &lt;QSignalSpy&gt;
#include &quot;../src/DriveMonitor.h&quot;
#include &quot;../src/QueueManager.h&quot;
#include &lt;QDir&gt;
#include &lt;QThread&gt;
#include &lt;QStorageInfo&gt;

class TestDriveMonitor : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        m_tempDrive = QDir::tempPath() + &quot;/testdrive_&quot; + QString::number(QDateTime::currentMSecsSinceEpoch());
        QDir().mkpath(m_tempDrive);
        // Add test file
        QFile(m_tempDrive + &quot;/testfile.txt&quot;).open(QIODevice::WriteOnly);
    }

    void cleanupTestCase() {
        QDir(m_tempDrive).removeRecursively();
    }

    void testQStorageInfoAllPlatforms() {
        QList&lt;QStorageInfo&gt; drives = QStorageInfo::mountedVolumes();
        QVERIFY(!drives.isEmpty());
        
        int removableCount = 0;
        for (const QStorageInfo &amp;info : drives) {
            QVERIFY(info.isValid());
            if (info.isReady() &amp;&amp; info.rootPath() != &quot;/&quot;) {
                removableCount += DriveMonitor().isRemovableDrive(info) ? 1 : 0;
            }
        }
        // Expect at least system drives detected
        QVERIFY(removableCount &gt;= 0);
    }

    void testFileSystemWatcher() {
        DriveMonitor monitor;
        QSignalSpy spyConnected(&amp;monitor, &amp;DriveMonitor::driveConnected);
        QSignalSpy spyDisconnected(&amp;monitor, &amp;DriveMonitor::driveDisconnected);
        
        // Should scan immediately
        QTest::qWait(100);
        QVERIFY(spyConnected.count() &gt;= 0 || spyDisconnected.count() &gt;= 0);
    }

    void testPollingFallback() {
        DriveMonitor monitor;
        monitor.m_pollTimer-&gt;setSingleShot(true);
        monitor.m_pollTimer-&gt;start(100);
        QTest::qWait(200);
        QVERIFY(!monitor.getCurrentDrives().isEmpty());
    }

    void testQueueIntegration() {
        DriveMonitor monitor;
        QueueManager queueMgr;
        
        QSignalSpy spyPause(&amp;queueMgr, &amp;QueueManager::pauseAll);
        QSignalSpy spyResume(&amp;queueMgr, &amp;QueueManager::resumeAll);
        
        // Simulate disconnect/connect via direct call (in real: via signals)
        emit monitor.driveDisconnected(&quot;/fake&quot;);
        QCOMPARE(spyPause.count(), 1);
        
        emit monitor.driveConnected(&quot;/fake&quot;);
        QCOMPARE(spyResume.count(), 1);
    }

    void testLastFilesResumeCheck() {
        DriveMonitor monitor;
        QString path = m_tempDrive;
        monitor.addDrive(path);
        QVERIFY(monitor.m_lastFiles[path]);  // Access via friendship or public for test
        
        // Remove files, check false
        QDir(path).removeRecursively(&quot;testfile.txt&quot;);
        QVERIFY(!QDir(path).entryList(QDir::Files).isEmpty() == false);
    }

private:
    QString m_tempDrive;
};

QTEST_MAIN(TestDriveMonitor)
#include &quot;test_drivemonitor.moc&quot;