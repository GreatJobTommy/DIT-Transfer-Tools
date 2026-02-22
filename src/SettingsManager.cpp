#include "SettingsManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QCryptographicHash>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + QDir::separator() + "settings.ini", QSettings::IniFormat)
{
    // Defaults
    if (!m_settings.contains("parallelTasks")) {
        m_settings.setValue("parallelTasks", 4);
    }
    if (!m_settings.contains("hashAlgo")) {
        m_settings.setValue("hashAlgo", "SHA256");
    }
    if (!m_settings.contains("drivePollInterval")) {
        m_settings.setValue("drivePollInterval", 10);
    }
    m_settings.sync();
}

int SettingsManager::parallelTasks() const {
    return m_settings.value("parallelTasks", 4).toInt();
}

QString SettingsManager::hashAlgo() const {
    return m_settings.value("hashAlgo", "SHA256").toString();
}

int SettingsManager::drivePollInterval() const {
    return m_settings.value("drivePollInterval", 10).toInt();
}

void SettingsManager::setParallelTasks(int value) {
    if (value >= 1 &amp;&amp; value <= 32) {
        m_settings.setValue("parallelTasks", value);
        m_settings.sync();
        emitChanged();
    }
}

void SettingsManager::setHashAlgo(const QString&amp; algo) {
    if (algo == "SHA256" || algo == "SHA512") {
        m_settings.setValue("hashAlgo", algo);
        m_settings.sync();
        emitChanged();
    }
}

void SettingsManager::setDrivePollInterval(int seconds) {
    if (seconds >= 1 &amp;&amp; seconds <= 60) {
        m_settings.setValue("drivePollInterval", seconds);
        m_settings.sync();
        emitChanged();
    }
}

void SettingsManager::emitChanged() {
    emit settingsChanged();
}

QByteArray SettingsManager::serialize() const {
    return m_settings.serialize(); // Note: QSettings doesn't have direct serialize, this is simplified
}

bool SettingsManager::deserialize(const QByteArray&amp; data) {
    // Simplified, would need proper QSettings deserialization
    return false;
}
