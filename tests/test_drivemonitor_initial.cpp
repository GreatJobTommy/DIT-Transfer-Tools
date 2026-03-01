/****************************************************************************
** test_drivemonitor_initial.cpp - Test for initial drive scan and listing
** Tests if drives already connected at startup are detected and listed.
****************************************************************************/

#include <QtTest>
#include <QSignalSpy>
#include "../src/DriveMonitor.h"

class TestDriveMonitorInitial : public QObject {
    Q_OBJECT

private slots:
    void testInitialDriveScan() {
        DriveMonitor monitor;
        QSignalSpy spyConnected(&monitor, &DriveMonitor::driveConnected);

        // Wait for initial scan to complete
        QTest::qWait(100);

        QList<QStorageInfo> drives = monitor.getCurrentDrives();
        int removableCount = 0;
        for (const QStorageInfo& info : drives) {
            if (monitor.isRemovableDrive(info)) {
                removableCount++;
            }
        }

        // Check that signals were emitted for all removable drives detected
        QCOMPARE(spyConnected.count(), removableCount);
    }
};

QTEST_MAIN(TestDriveMonitorInitial)
#include "test_drivemonitor_initial.moc"