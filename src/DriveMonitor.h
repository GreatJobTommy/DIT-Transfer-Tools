#ifndef DRIVEMONITOR_H
#define DRIVEMONITOR_H

#include <QObject>
#include <QStorageInfo>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QMap>
#include <QStringList>

class DriveMonitor : public QObject {
    Q_OBJECT

public:
    explicit DriveMonitor(QObject* parent = nullptr);
    ~DriveMonitor();

    QList<QStorageInfo> getCurrentDrives() const;
    QStringList getAvailableDrives();
    bool isRemovableDrive(const QStorageInfo& info) const;
    void addDrive(const QString& path);
    void removeDrive(const QString& path);

signals:
    void driveConnected(const QString& path);
    void driveDisconnected(const QString& path);
    void driveReconnected();
    void driveAdded(const QString& drive);
    void driveRemoved(const QString& drive);

private slots:
    void onDirectoryChanged(const QString& path);
    void pollDrives();
    void checkDrives();

private:
    void scanDrives();

    QFileSystemWatcher* m_watcher;
    QTimer* m_pollTimer;
    QList<QStorageInfo> m_currentDrives;
    QStringList m_currentDrivesList;
public:
    QMap<QString, bool> m_lastFiles; // For resume check, public for test
};

#endif // DRIVEMONITOR_H