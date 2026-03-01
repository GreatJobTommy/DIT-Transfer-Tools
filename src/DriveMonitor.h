#ifndef DRIVEMONITOR_H
#define DRIVEMONITOR_H

#include <QObject>
#include <QStorageInfo>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QMap>

class DriveMonitor : public QObject {
    Q_OBJECT

public:
    explicit DriveMonitor(QObject* parent = nullptr);
    ~DriveMonitor();

    QList<QStorageInfo> getCurrentDrives() const;
    bool isRemovableDrive(const QStorageInfo& info) const;
    void addDrive(const QString& path);
    void removeDrive(const QString& path);

signals:
    void driveConnected(const QString& path);
    void driveDisconnected(const QString& path);

private slots:
    void onDirectoryChanged(const QString& path);
    void pollDrives();

private:
    void scanDrives();

    QFileSystemWatcher* m_watcher;
    QTimer* m_pollTimer;
    QList<QStorageInfo> m_currentDrives;
public:
    QMap<QString, bool> m_lastFiles; // For resume check, public for test
};

#endif // DRIVEMONITOR_H