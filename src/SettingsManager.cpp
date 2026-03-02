#include "SettingsManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

SettingsManager::SettingsManager(QObject* parent)
    : QObject(parent), m_settings(new QSettings("DIT", "TransferTools", this)) {}

SettingsManager::~SettingsManager() {}

QVariant SettingsManager::getSetting(const QString& key, const QVariant& defaultValue) const {
    return m_settings->value(key, defaultValue);
}

void SettingsManager::setSetting(const QString& key, const QVariant& value) {
    m_settings->setValue(key, value);
    emit settingChanged(key, value);
}

int SettingsManager::getMaxParallelTransfers() const {
    return getSetting("maxParallel", 4).toInt();
}

void SettingsManager::setMaxParallelTransfers(int value) {
    setSetting("maxParallel", value);
}

bool SettingsManager::getAutoStart() const {
    return getSetting("autoStart", false).toBool();
}

void SettingsManager::setAutoStart(bool value) {
    setSetting("autoStart", value);
}

// New settings
QString SettingsManager::getHashAlgorithm() const {
    return getSetting("hashAlgorithm", "sha256").toString();
}

void SettingsManager::setHashAlgorithm(const QString& algo) {
    setSetting("hashAlgorithm", algo);
}

int SettingsManager::getTransferChunkSize() const {
    return getSetting("transferChunkSize", 0).toInt(); // 0 = auto
}

void SettingsManager::setTransferChunkSize(int size) {
    setSetting("transferChunkSize", size);
}

int SettingsManager::getDrivesScanInterval() const {
    return getSetting("drivesScanInterval", 10).toInt(); // seconds
}

void SettingsManager::setDrivesScanInterval(int interval) {
    setSetting("drivesScanInterval", interval);
}

bool SettingsManager::getUIThemeDark() const {
    return getSetting("uiThemeDark", false).toBool();
}

void SettingsManager::setUIThemeDark(bool dark) {
    setSetting("uiThemeDark", dark);
}

QString SettingsManager::getLogsLevel() const {
    return getSetting("logsLevel", "info").toString();
}

void SettingsManager::setLogsLevel(const QString& level) {
    setSetting("logsLevel", level);
}

QStringList SettingsManager::getPresets() const {
    return getSetting("presets", QStringList()).toStringList();
}

void SettingsManager::setPresets(const QStringList& presets) {
    setSetting("presets", presets);
}

void SettingsManager::addPreset(const QString& preset) {
    QStringList presets = getPresets();
    if (!presets.contains(preset)) {
        presets.append(preset);
        setPresets(presets);
    }
}

void SettingsManager::removePreset(const QString& preset) {
    QStringList presets = getPresets();
    presets.removeAll(preset);
    setPresets(presets);
}

bool SettingsManager::getAdvancedRclone() const {
    return getSetting("advancedRclone", false).toBool();
}

void SettingsManager::setAdvancedRclone(bool enabled) {
    setSetting("advancedRclone", enabled);
}

// QJson save/load
bool SettingsManager::saveToJson(const QString& filePath) {
    QJsonObject json;
    // Add all settings to json
    for (const QString& key : m_settings->allKeys()) {
        json[key] = QJsonValue::fromVariant(m_settings->value(key));
    }

    QJsonDocument doc(json);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        return true;
    }
    return false;
}

bool SettingsManager::loadFromJson(const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject json = doc.object();
            for (auto it = json.begin(); it != json.end(); ++it) {
                setSetting(it.key(), it.value().toVariant());
            }
            return true;
        }
    }
    return false;
}

void SettingsManager::resetToDefaults() {
    m_settings->clear();
    emit settingChanged("", QVariant()); // Signal reset
}

void SettingsManager::setValue(const QString& key, const QVariant& value) {
    setSetting(key, value);
}

QVariant SettingsManager::value(const QString& key, const QVariant& defaultValue) {
    return getSetting(key, defaultValue);
}