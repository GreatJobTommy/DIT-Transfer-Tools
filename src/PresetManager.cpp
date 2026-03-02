#include "PresetManager.h"
#include <QDebug>

PresetManager::PresetManager(QObject* parent)
    : QObject(parent) {
    createDefaultPresets();
}

PresetManager::~PresetManager() {}

bool PresetManager::savePreset(const Preset& preset) {
    QJsonObject json;
    json["name"] = preset.name;
    json["description"] = preset.description;
    json["settings"] = preset.settings;
    json["tasks"] = preset.tasks;

    QDir dir(getPresetsDir());
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile file(getPresetFilePath(preset.name));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << file.fileName();
        return false;
    }

    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    emit presetSaved(preset.name);
    return true;
}

bool PresetManager::loadPreset(const QString& name, Preset& preset) {
    QFile file(getPresetFilePath(name));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << file.fileName();
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }

    QJsonObject json = doc.object();
    preset.name = json["name"].toString();
    preset.description = json["description"].toString();
    preset.settings = json["settings"].toObject();
    preset.tasks = json["tasks"].toArray();

    emit presetLoaded(name);
    return true;
}

QStringList PresetManager::getPresetNames() const {
    QDir dir(getPresetsDir());
    return dir.entryList(QStringList() << "*.json", QDir::Files);
}

bool PresetManager::deletePreset(const QString& name) {
    QFile file(getPresetFilePath(name));
    if (file.remove()) {
        emit presetDeleted(name);
        return true;
    }
    return false;
}

void PresetManager::createDefaultPresets() {
    // USB Safe preset
    Preset usbSafe;
    usbSafe.name = "USB Safe";
    usbSafe.description = "Safe USB transfer with multi-target hashing and verification";

    QJsonObject settings;
    settings["maxParallel"] = 2;
    settings["chunkSize"] = 1024 * 1024; // 1MB
    settings["hashAlgorithm"] = "SHA256";
    settings["verifyAfterTransfer"] = true;
    settings["multiTarget"] = true;
    usbSafe.settings = settings;

    QJsonArray tasks;
    QJsonObject task1;
    task1["source"] = "/source/path";
    task1["destinations"] = QJsonArray({"/usb1", "/usb2"});
    task1["type"] = "multi";
    tasks.append(task1);
    usbSafe.tasks = tasks;

    savePreset(usbSafe);
}

QString PresetManager::getPresetsDir() const {
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/presets";
}

QString PresetManager::getPresetFilePath(const QString& name) const {
    return getPresetsDir() + "/" + name + ".json";
}