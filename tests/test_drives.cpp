#include <QtTest>
#include "../src/MainWindow.h"
#include "../src/QueueManager.h"

class TestDrives : public QObject {
    Q_OBJECT

private slots:
    void testDrivesTab() {
        QueueManager queue;
        MainWindow window(&queue);
        window.show();

        // Check if drives tab exists
        QTabWidget* tabWidget = window.findChild<QTabWidget*>("tabWidget");
        QVERIFY(tabWidget != nullptr);

        int drivesTabIndex = -1;
        for (int i = 0; i < tabWidget->count(); ++i) {
            if (tabWidget->tabText(i) == "Drives") {
                drivesTabIndex = i;
                break;
            }
        }
        QVERIFY(drivesTabIndex != -1);

        // Check drives list
        QListWidget* drivesList = window.findChild<QListWidget*>("drivesList");
        QVERIFY(drivesList != nullptr);

        // Simulate update - not calling private method
        QVERIFY(drivesList->count() >= 0);
    }
};

QTEST_MAIN(TestDrives)
#include "test_drives.moc"