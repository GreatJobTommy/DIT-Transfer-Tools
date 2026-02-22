#include "ConfigManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <QApplication>
ConfigManager* ConfigManager::m_instance = nullptr;
ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_qsettings(new QSettings("DIT-Transfer-Tools", "Config", this))
    , m_watcher(new QFileSystemWatcher(this))
    , m_jsonBackupPath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.json")
{
    loadDefaults();
    migrateFromOldFormat();
    loadAllSettings();
    
    m_watcher->addPath(m_jsonBackupPath);
    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &ConfigManager::onJsonFileChanged);
}
ConfigManager::~ConfigManager() {
    saveAllSettings();
    m_instance = nullptr;
}
ConfigManager* ConfigManager::instance() {
    if (!m_instance) {
        m_instance = new ConfigManager();
    }
    return m_instance;
}
void ConfigManager::loadAllSettings() {
    loadFromJson(m_jsonBackupPath);
    m_qsettings->sync();
    emit settingsLoaded();
}
void ConfigManager::saveAllSettings() {
    saveToJson(m_jsonBackupPath);
    m_qsettings->sync();
    emit settingsSaved();
}
QString ConfigManager::getTransferDir() const {
    return m_qsettings->value("transfer/dir", QDir::homePath()).toString();
}
void ConfigManager::setTransferDir(const QString &dir) {
    m_qsettings->setValue("transfer/dir", dir);
    emit transferDirChanged(dir);
}
QString ConfigManager::getHashAlgorithm() const {
    return m_qsettings->value("hash/algorithm", "SHA256").toString();
}
void ConfigManager::setHashAlgorithm(const QString &algo) {
    m_qsettings->setValue("hash/algorithm", algo);
    emit hashAlgorithmChanged(algo);
}
bool ConfigManager::getAutoTransfer() const {
    return m_qsettings->value("transfer/auto", false).toBool();
}
void ConfigManager::setAutoTransfer(bool enabled) {
    m_qsettings->setValue("transfer/auto", enabled);
    emit autoTransferChanged(enabled);
}
void ConfigManager::loadDefaults() {
    m_qsettings->setValue("transfer/dir", QDir::homePath());
    m_qsettings->setValue("hash/algorithm", "SHA256");
    m_qsettings->setValue("transfer/auto", false);
}
void ConfigManager::migrateFromOldFormat() {
    // Simple migration example
    if (m_qsettings->contains("oldTransferPath")) {
        setTransferDir(m_qsettings->value("oldTransferPath").toString());
        m_qsettings->remove("oldTransferPath");
    }
}
bool ConfigManager::loadFromJson(const QString &jsonPath) {
    QFile file(jsonPath);
    if (!file.exists()) {
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        recoverFromCorruptJson();
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        recoverFromCorruptJson();
        return false;
    }
    
    jsonToSettings(doc.object());
    return true;
}
bool ConfigManager::saveToJson(const QString &jsonPath) {
    QJsonObject json = settingsToJson();
    QJsonDocument doc(json);
    QFile file(jsonPath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(doc.toJson());
    return true;
}
void ConfigManager::recoverFromCorruptJson() {
    qDebug() << "Recovering from corrupt JSON, loading defaults";
    loadDefaults();
}
QJsonObject ConfigManager::settingsToJson() {
    QJsonObject json;
    json["transfer/dir"] = getTransferDir();
    json["hash/algorithm"] = getHashAlgorithm();
    json["transfer/auto"] = getAutoTransfer();
    return json;
}
void ConfigManager::jsonToSettings(const QJsonObject &json) {
    if (json.contains("transfer/dir")) setTransferDir(json["transfer/dir"].toString());
    if (json.contains("hash/algorithm")) setHashAlgorithm(json["hash/algorithm"].toString());
    if (json.contains("transfer/auto")) setAutoTransfer(json["transfer/auto"].toBool());
}
void ConfigManager::onJsonFileChanged(const QString &path) {
    loadAllSettings();
}
