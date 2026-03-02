#include "SettingsManager.h"

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

void SettingsManager::setValue(const QString& key, const QVariant& value) {
    setSetting(key, value);
}

QVariant SettingsManager::value(const QString& key, const QVariant& defaultValue) {
    return getSetting(key, defaultValue);
}