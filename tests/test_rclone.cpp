#include <QCoreApplication>
#include <QTest>
#include <QSignalSpy>
#include "../src/TransferTask.h"
#include "../src/SettingsManager.h"

class TestRclone : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Setup
    }

    void testIsRcloneRemote() {
        TransferTask task("/local/source", "/local/dest");
        QVERIFY(!task.isRcloneRemote());

        TransferTask remoteTask("/local/source", "gdrive:/folder");
        QVERIFY(remoteTask.isRcloneRemote());

        TransferTask s3Task("/local/source", "s3:/bucket");
        QVERIFY(s3Task.isRcloneRemote());
    }

    void testRcloneConfigPath() {
        SettingsManager settings;
        QString defaultPath = settings.getRcloneConfigPath();
        QVERIFY(!defaultPath.isEmpty());

        QString customPath = "/custom/rclone.conf";
        settings.setRcloneConfigPath(customPath);
        QCOMPARE(settings.getRcloneConfigPath(), customPath);
    }

    void cleanupTestCase() {
        // Cleanup
    }
};

QTEST_MAIN(TestRclone)
#include "test_rclone.moc"