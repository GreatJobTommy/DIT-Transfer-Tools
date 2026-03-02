#ifndef DRIVEMONITOR_H
#define DRIVEMONITOR_H

#include <QObject>
#include <QStorageInfo>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QMap>
#include <QStringList>
#include <QString>

struct ResumeInfo {
    QString lastFile;
    qint64 offset;
};

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
    void setResumeOffset(const QString& path, const QString& lastFile, qint64 offset);

signals:
    void driveConnected(const QString& path);
    void driveDisconnected(const QString& path);
    void driveReconnected();
    void driveReconnected(const QString& path, const QString& lastFile, qint64 offset);
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
    QSet<QString> m_removedDrives;
public:
    QMap<QString, bool> m_lastFiles; // For resume check, public for test
    QMap<QString, ResumeInfo> m_resumeOffsets;

signals:
    void driveReconnected(const QString& path, const QString& lastFile, qint64 offset);
};

#endif // DRIVEMONITOR_H