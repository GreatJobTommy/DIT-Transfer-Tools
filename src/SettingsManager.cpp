#include "SettingsManager.h"

SettingsManager::SettingsManager(QObject *parent) : QObject(parent) {
    m_settings = new QSettings("DIT", "TransferTools", this);
}

SettingsManager::~SettingsManager() {}

QVariant SettingsManager::value(const QString& key, const QVariant& defaultValue) const {
    return m_settings->value(key, defaultValue);
}

void SettingsManager::setValue(const QString& key, const QVariant& value) {
    m_settings->setValue(key, value);
}

void SettingsManager::loadFromFile(const QString& filePath) {
    // For config.ini, perhaps use QSettings with IniFormat
    QSettings* iniSettings = new QSettings(filePath, QSettings::IniFormat);
    // Copy values to m_settings
    for (const QString& key : iniSettings->allKeys()) {
        m_settings->setValue(key, iniSettings->value(key));
    }
    delete iniSettings;
}

void SettingsManager::saveToFile(const QString& filePath) {
    QSettings* iniSettings = new QSettings(filePath, QSettings::IniFormat);
    for (const QString& key : m_settings->allKeys()) {
        iniSettings->setValue(key, m_settings->value(key));
    }
    delete iniSettings;
}