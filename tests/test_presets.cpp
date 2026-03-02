#include <QtTest/QtTest>
#include <QApplication>
#include "../src/PresetManager.h"

class TestPresets : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testSaveLoadPreset();
    void testDefaultPresets();
    void testDeletePreset();
    void cleanupTestCase();

private:
    PresetManager* m_presetManager;
};

void TestPresets::initTestCase() {
    m_presetManager = new PresetManager(this);
}

void TestPresets::testSaveLoadPreset() {
    PresetManager::Preset preset;
    preset.name = "Test Preset";
    preset.description = "A test preset";

    QJsonObject settings;
    settings["maxParallel"] = 3;
    preset.settings = settings;

    QJsonArray tasks;
    QJsonObject task;
    task["source"] = "/src";
    task["destinations"] = QJsonArray({"/dst1", "/dst2"});
    tasks.append(task);
    preset.tasks = tasks;

    QVERIFY(m_presetManager->savePreset(preset));

    PresetManager::Preset loadedPreset;
    QVERIFY(m_presetManager->loadPreset("Test Preset", loadedPreset));
    QCOMPARE(loadedPreset.name, preset.name);
    QCOMPARE(loadedPreset.settings["maxParallel"].toInt(), 3);
}

void TestPresets::testDefaultPresets() {
    QStringList names = m_presetManager->getPresetNames();
    QVERIFY(names.contains("USB Safe.json"));
}

void TestPresets::testDeletePreset() {
    PresetManager::Preset preset;
    preset.name = "To Delete";
    m_presetManager->savePreset(preset);

    QVERIFY(m_presetManager->deletePreset("To Delete"));
    QStringList names = m_presetManager->getPresetNames();
    QVERIFY(!names.contains("To Delete.json"));
}

void TestPresets::cleanupTestCase() {
    delete m_presetManager;
}

QTEST_MAIN(TestPresets)
#include "test_presets.moc"