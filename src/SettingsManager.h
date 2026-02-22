#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include &lt;QObject&gt;
#include &lt;QSettings&gt;
#include &lt;QHash&gt;
#include &lt;QByteArray&gt;

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
    void setHashAlgo(const QString&amp; algo);
    void setDrivePollInterval(int seconds);

    QByteArray serialize() const;
    bool deserialize(const QByteArray&amp; data);

signals:
    void settingsChanged();

private:
    QSettings m_settings;
    void emitChanged();

};

#endif // SETTINGS_MANAGER_H
