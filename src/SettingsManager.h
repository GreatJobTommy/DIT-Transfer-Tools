#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H
#include <QObject>
#include <QSettings>
#include <QHash>
#include <QByteArray>
class SettingsManager : public QObject {
    Q_OBJECT
public:
    explicit SettingsManager(QObject *parent = nullptr);
    // Getters
    int parallelTasks() const;
    QString hashAlgo() const;
    int drivePollInterval() const;
    // Setters
    void setParallelTasks(int value);
    void setHashAlgo(const QString& algo);
    void setDrivePollInterval(int seconds);
    QByteArray serialize() const;
    bool deserialize(const QByteArray& data);
signals:
    void settingsChanged();
private:
    QSettings m_settings;
    void emitChanged();
};
#endif // SETTINGS_MANAGER_H
