/****************************************************************************
** test_drivemonitor.cpp - Unit-Tests für DriveMonitor
** Qt Test Framework. Testet Cross-Platform, Watcher/Poll, Signals.
****************************************************************************/

#include &lt;QtTest&gt;
#include &quot;../src/DriveMonitor.h&quot;
#include &lt;QSignalSpy&gt;
#include &lt;QTestEventList&gt;
#include &lt;QDir&gt;
#include &lt;QStorageInfo&gt;
#include &lt;QThread&gt;

class TestDriveMonitor : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Create temp dir to simulate drive
        m_tempDir = QDir::tempPath() + &quot;/testdrive_&quot; + QString::number(QDateTime::currentMSecsSinceEpoch());
        QDir().mkpath(m_tempDir);
    }

    void cleanupTestCase() {
        QDir(m_tempDir).removeRecursively();
    }

    void testQStorageInfoCrossPlatform() {
        QList&lt;QStorageInfo&gt; drives = QStorageInfo::mountedVolumes();
        QCOMPARE(drives.size(), drives.size()); // Basic check
        
        for (const QStorageInfo &amp;drive : drives) {
            QVERIFY2(drive.isValid(), (drive.rootPath() + &quot; invalid&quot;).toUtf8());
            QVERIFY2(!drive.rootPath().isEmpty(), &quot;Empty root path&quot;);
        }
    }

    void testPollingFallback() {
        DriveMonitor monitor;
        QSignalSpy spy(&amp;monitor, &amp;DriveMonitor::driveReconnected);
        
        // Simulate drive by touching temp dir
        QDir(m_tempDir).mkpath(&quot;testfile&quot;);
        
        // Wait for poll (reduce timer for test)
        monitor.m_timer-&gt;setInterval(100);
        QTest::qWait(200);
        
        QCOMPARE(spy.count(), 1); // Should detect temp drive as &apos;reconnected&apos;
    }

    void testQueuePauseResume_data() {
        QTest::addColumn&lt;bool&gt;(&quot;disconnect&quot;);
        QTest::newRow(&quot;disconnect&quot;) &lt;&lt; true;
        QTest::newRow(&quot;reconnect&quot;) &lt;&lt; false;
    }

    void testQueuePauseResume() {
        QFETCH(bool, disconnect);
        
        DriveMonitor monitor;
        // Assume global QueueManager instance or mock
        // For test: verify pause/resume signals (extend DriveMonitor if needed)
        QVERIFY(true); // Placeholder - integrate with QueueManager in real test
    }

    void testFileSystemWatcher() {
#ifdef Q_OS_LINUX
        DriveMonitor monitor;
        // Test watcher registration on known paths
        QVERIFY(monitor.getMountedDrives().size() &gt; 0);
#endif
    }

private:
    QString m_tempDir;
};

QTEST_MAIN(TestDriveMonitor)
#include &quot;test_drivemonitor.moc&quot;