#ifndef DRIVEMONITOR_H
#define DRIVEMONITOR_H

#include <QObject>
#include <QStorageInfo>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QMap>
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
    bool isRemovableDrive(const QStorageInfo& info) const;
    void addDrive(const QString& path);
    void removeDrive(const QString& path);
    void setResumeOffset(const QString& path, const QString& lastFile, qint64 offset);

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
    QSet<QString> m_removedDrives;
public:
    QMap<QString, bool> m_lastFiles; // For resume check, public for test
    QMap<QString, ResumeInfo> m_resumeOffsets;

signals:
    void driveReconnected(const QString& path, const QString& lastFile, qint64 offset);
};

#endif // DRIVEMONITOR_H