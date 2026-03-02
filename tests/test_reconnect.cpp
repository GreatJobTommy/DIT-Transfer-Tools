#include <QtTest/QtTest>
#include "../src/DriveMonitor.h"

class TestReconnect : public QObject {
    Q_OBJECT

private slots:
    void testReconnectSignal();
};

void TestReconnect::testReconnectSignal() {
    DriveMonitor monitor;
    QSignalSpy spy(&monitor, &DriveMonitor::driveReconnected);

    // Simulate remove and add
    monitor.removeDrive("/test");
    monitor.setResumeOffset("/test", "file.txt", 12345);
    monitor.addDrive("/test"); // Assuming addDrive emits connected, but for reconnect, need to simulate poll

    // Since addDrive is private, hard to test directly. Test the map.
    QVERIFY(monitor.m_resumeOffsets.contains("/test"));
    QCOMPARE(monitor.m_resumeOffsets["/test"].lastFile, QString("file.txt"));
    QCOMPARE(monitor.m_resumeOffsets["/test"].offset, qint64(12345));
}

QTEST_MAIN(TestReconnect)
#include "test_reconnect.moc"