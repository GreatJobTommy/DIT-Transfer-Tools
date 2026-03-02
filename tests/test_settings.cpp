#include <QCoreApplication>
#include <QTest>
#include <QSignalSpy>
#include "../src/SettingsManager.h"

class TestSettings : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "Initializing test case";
    }

    void testDefaultValues() {
        SettingsManager settings;
        QCOMPARE(settings.getMaxParallelTransfers(), 4);
        QCOMPARE(settings.getAutoStart(), false);
        QCOMPARE(settings.getHashAlgorithm(), QString("sha256"));
        QCOMPARE(settings.getTransferChunkSize(), 0);
        QCOMPARE(settings.getDrivesScanInterval(), 10);
        QCOMPARE(settings.getUIThemeDark(), false);
        QCOMPARE(settings.getLogsLevel(), QString("info"));
        QCOMPARE(settings.getPresets().isEmpty(), true);
        QCOMPARE(settings.getAdvancedRclone(), false);
    }

    void testSetGetValues() {
        SettingsManager settings;
        QSignalSpy spy(&settings, &SettingsManager::settingChanged);

        settings.setMaxParallelTransfers(8);
        QCOMPARE(settings.getMaxParallelTransfers(), 8);
        QCOMPARE(spy.count(), 1);

        settings.setHashAlgorithm("sha512");
        QCOMPARE(settings.getHashAlgorithm(), QString("sha512"));

        settings.setTransferChunkSize(50);
        QCOMPARE(settings.getTransferChunkSize(), 50);

        settings.setDrivesScanInterval(30);
        QCOMPARE(settings.getDrivesScanInterval(), 30);

        settings.setUIThemeDark(true);
        QCOMPARE(settings.getUIThemeDark(), true);

        settings.setLogsLevel("debug");
        QCOMPARE(settings.getLogsLevel(), QString("debug"));

        QStringList presets = {"preset1", "preset2"};
        settings.setPresets(presets);
        QCOMPARE(settings.getPresets(), presets);

        settings.setAdvancedRclone(true);
        QCOMPARE(settings.getAdvancedRclone(), true);
    }

    void testJsonSaveLoad() {
        SettingsManager settings;
        settings.setMaxParallelTransfers(10);
        settings.setHashAlgorithm("sha512");

        QString tempFile = "test_settings.json";
        QVERIFY(settings.saveToJson(tempFile));

        SettingsManager settings2;
        QVERIFY(settings2.loadFromJson(tempFile));

        QCOMPARE(settings2.getMaxParallelTransfers(), 10);
        QCOMPARE(settings2.getHashAlgorithm(), QString("sha512"));

        // Clean up
        QFile::remove(tempFile);
    }

    void testResetToDefaults() {
        SettingsManager settings;
        settings.setMaxParallelTransfers(15);
        settings.setHashAlgorithm("sha512");

        settings.resetToDefaults();

        QCOMPARE(settings.getMaxParallelTransfers(), 4);
        QCOMPARE(settings.getHashAlgorithm(), QString("sha256"));
    }

    void testPresets() {
        SettingsManager settings;
        settings.addPreset("test1");
        settings.addPreset("test2");
        QCOMPARE(settings.getPresets().size(), 2);

        settings.removePreset("test1");
        QCOMPARE(settings.getPresets().size(), 1);
        QCOMPARE(settings.getPresets().first(), QString("test2"));
    }

    void cleanupTestCase() {
        qDebug() << "Cleaning up test case";
    }
};

QTEST_MAIN(TestSettings)
#include "test_settings.moc"