#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QVariant>

class SettingsManager : public QObject {
    Q_OBJECT

public:
    explicit SettingsManager(QObject* parent = nullptr);
    ~SettingsManager();

    QVariant getSetting(const QString& key, const QVariant& defaultValue = QVariant()) const;
    void setSetting(const QString& key, const QVariant& value);

    // Specific settings
    int getMaxParallelTransfers() const;
    void setMaxParallelTransfers(int value);

    bool getAutoStart() const;
    void setAutoStart(bool value);

    QString getRcloneConfigPath() const;
    void setRcloneConfigPath(const QString& path);

    // Notification settings
    bool getNotificationsEnabled() const;
    void setNotificationsEnabled(bool value);

    bool getNotificationSoundEnabled() const;
    void setNotificationSoundEnabled(bool value);

    QString getNotificationSoundFile() const;
    void setNotificationSoundFile(const QString& file);

    bool getUseSystemNotifications() const;
    void setUseSystemNotifications(bool value);

    // Logging settings
    int getLogLevel() const;
    void setLogLevel(int level);

    QString getLogPath() const;
    void setLogPath(const QString& path);

    void setValue(const QString& key, const QVariant& value);
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant());

signals:
    void settingChanged(const QString& key, const QVariant& value);

private:
    QSettings* m_settings;
};

#endif // SETTINGSMANAGER_H