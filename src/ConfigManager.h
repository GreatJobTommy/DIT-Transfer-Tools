#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H
#include <QObject>
#include <QSettings>
#include <QJsonObject>
#include <QFileSystemWatcher>
class ConfigManager : public QObject {
    Q_OBJECT
public:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();
    static ConfigManager* instance();
    void loadAllSettings();
    void saveAllSettings();
    
    // Getters/Setters for settings with live signals
    QString getTransferDir() const;
    void setTransferDir(const QString &dir);
    
    QString getHashAlgorithm() const;
    void setHashAlgorithm(const QString &algo);
    
    bool getAutoTransfer() const;
    void setAutoTransfer(bool enabled);
    
    // Migration from old settings
    void migrateFromOldFormat();
signals:
    void transferDirChanged(const QString &dir);
    void hashAlgorithmChanged(const QString &algo);
    void autoTransferChanged(bool enabled);
    void settingsLoaded();
    void settingsSaved();
private slots:
    void onJsonFileChanged(const QString &path);
private:
    void loadDefaults();
    bool loadFromJson(const QString &jsonPath);
    bool saveToJson(const QString &jsonPath);
    void recoverFromCorruptJson();
    QJsonObject settingsToJson();
    void jsonToSettings(const QJsonObject &json);
    static ConfigManager* m_instance;
    QSettings* m_qsettings;
    QFileSystemWatcher* m_watcher;
    QString m_jsonBackupPath;
};
#endif // CONFIGMANAGER_H
