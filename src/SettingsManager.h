#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QVariant>
#include <QStringList>

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

    // New settings for UI
    QString getHashAlgorithm() const;
    void setHashAlgorithm(const QString& algo);

    int getTransferChunkSize() const; // in MB, 0 for auto
    void setTransferChunkSize(int size);

    int getDrivesScanInterval() const; // in seconds
    void setDrivesScanInterval(int interval);

    bool getUIThemeDark() const;
    void setUIThemeDark(bool dark);

    QString getLogsLevel() const; // "debug", "info", "warning", "error"
    void setLogsLevel(const QString& level);

    QStringList getPresets() const;
    void setPresets(const QStringList& presets);
    void addPreset(const QString& preset);
    void removePreset(const QString& preset);

    bool getAdvancedRclone() const;
    void setAdvancedRclone(bool enabled);

    // QJson save/load
    bool saveToJson(const QString& filePath);
    bool loadFromJson(const QString& filePath);
    void resetToDefaults();

    void setValue(const QString& key, const QVariant& value);
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant());

signals:
    void settingChanged(const QString& key, const QVariant& value);

private:
    QSettings* m_settings;
};

#endif // SETTINGSMANAGER_H