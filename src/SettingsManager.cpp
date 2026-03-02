#include "SettingsManager.h"
#include <QStandardPaths>

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

QString SettingsManager::getRcloneConfigPath() const {
    return getSetting("rclone/configPath", QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/rclone/rclone.conf").toString();
}

void SettingsManager::setRcloneConfigPath(const QString& path) {
    setSetting("rclone/configPath", path);
}

// Notification settings
bool SettingsManager::getNotificationsEnabled() const {
    return getSetting("notifications/enabled", true).toBool();
}

void SettingsManager::setNotificationsEnabled(bool value) {
    setSetting("notifications/enabled", value);
}

bool SettingsManager::getNotificationSoundEnabled() const {
    return getSetting("notifications/soundEnabled", true).toBool();
}

void SettingsManager::setNotificationSoundEnabled(bool value) {
    setSetting("notifications/soundEnabled", value);
}

QString SettingsManager::getNotificationSoundFile() const {
    return getSetting("notifications/soundFile", "").toString();
}

void SettingsManager::setNotificationSoundFile(const QString& file) {
    setSetting("notifications/soundFile", file);
}

bool SettingsManager::getUseSystemNotifications() const {
    return getSetting("notifications/useSystem", false).toBool();
}

void SettingsManager::setUseSystemNotifications(bool value) {
    setSetting("notifications/useSystem", value);
}

void SettingsManager::setValue(const QString& key, const QVariant& value) {
    setSetting(key, value);
}

QVariant SettingsManager::value(const QString& key, const QVariant& defaultValue) {
    return getSetting(key, defaultValue);
}