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

// Watchdog settings
bool SettingsManager::getWatchdogEnabled() const {
    return getSetting("watchdog/enabled", false).toBool();
}

void SettingsManager::setWatchdogEnabled(bool value) {
    setSetting("watchdog/enabled", value);
}

QStringList SettingsManager::getWatchedFolders() const {
    return getSetting("watchdog/folders", QStringList()).toStringList();
}

void SettingsManager::setWatchedFolders(const QStringList& folders) {
    setSetting("watchdog/folders", folders);
}

int SettingsManager::getWatchdogPollInterval() const {
    return getSetting("watchdog/pollInterval", 5000).toInt();
}

void SettingsManager::setWatchdogPollInterval(int ms) {
    setSetting("watchdog/pollInterval", ms);
}

QStringList SettingsManager::getWatchdogFileFilters() const {
    return getSetting("watchdog/fileFilters", QStringList{"*.dpx"}).toStringList();
}

void SettingsManager::setWatchdogFileFilters(const QStringList& filters) {
    setSetting("watchdog/fileFilters", filters);
}

qint64 SettingsManager::getWatchdogMinFileSize() const {
    return getSetting("watchdog/minFileSize", 1000000000LL).toLongLong(); // 1GB
}

void SettingsManager::setWatchdogMinFileSize(qint64 bytes) {
    setSetting("watchdog/minFileSize", bytes);
}

QString SettingsManager::getWatchdogPreset() const {
    return getSetting("watchdog/preset", "default").toString();
}

void SettingsManager::setWatchdogPreset(const QString& preset) {
    setSetting("watchdog/preset", preset);
}

QString SettingsManager::getWatchdogDefaultDest() const {
    return getSetting("watchdog/defaultDest", QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/DIT_Ingest").toString();
}

void SettingsManager::setWatchdogDefaultDest(const QString& dest) {
    setSetting("watchdog/defaultDest", dest);
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

// Logging settings
int SettingsManager::getLogLevel() const {
    return getSetting("logging/level", 1).toInt(); // Default INFO
}

void SettingsManager::setLogLevel(int level) {
    setSetting("logging/level", level);
}

QString SettingsManager::getLogPath() const {
    return getSetting("logging/path", QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/dit-transfer-tools.log").toString();
}

void SettingsManager::setLogPath(const QString& path) {
    setSetting("logging/path", path);
}

void SettingsManager::setValue(const QString& key, const QVariant& value) {
    setSetting(key, value);
}

QVariant SettingsManager::value(const QString& key, const QVariant& defaultValue) {
    return getSetting(key, defaultValue);
}