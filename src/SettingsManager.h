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

signals:
    void settingChanged(const QString& key, const QVariant& value);

private:
    QSettings* m_settings;
};

#endif // SETTINGSMANAGER_H