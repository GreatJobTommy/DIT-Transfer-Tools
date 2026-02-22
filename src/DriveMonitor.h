/****************************************************************************
** DriveMonitor.h - Überwacht USB/Laufwerke cross-platform V3
** QStorageInfo + QFileSystemWatcher + df-poll fallback
** Signals: connected/disconnected, drivesChanged
** Max 250 Zeilen.
****************************************************************************/

#ifndef DRIVEMONITOR_H
#define DRIVEMONITOR_H

<<<<<<< HEAD
#include <QObject>
#include <QTimer>
#include <QSet>
#include <QMap>
#include <QFileSystemWatcher>
#include <QStorageInfo>
#include <QStringList>
=======
#include <QObject>
#include <QTimer>
#include <QSet>
#include <QMap>
#include <QFileSystemWatcher>
#include <QStorageInfo>
#include <QStringList>
#include "ErrorManager.h"
>>>>>>> origin/main

class DriveMonitor : public QObject
{
    Q_OBJECT

public:
    explicit DriveMonitor(QObject *parent = nullptr);
    ~DriveMonitor();

    QStringList getCurrentDrives() const { return m_currentDrives; }
    bool isDriveWatched(const QString &path) const;

signals:
    void driveConnected(const QString &drivePath);
    void driveDisconnected(const QString &drivePath);
    void drivesChanged(const QStringList &availableDrives);

private slots:
    void checkDrivesPoll();
    void onStorageChanged();
    void onWatcherChange(const QString &path);

private:
    void scanDrives();
    bool isRemovableDrive(const QStorageInfo &info) const;
    void addDrive(const QString &path);
    void removeDrive(const QString &path);
    QStringList getFallbackDrives() const;  // df/wmic fallback

    QTimer *m_pollTimer;
    QFileSystemWatcher *m_watcher;
    QSet<QString> m_knownDrives;
    QStringList m_currentDrives;
    QMap<QString, bool> m_lastFiles;  // For resume check: path -> had files
};

#endif // DRIVEMONITOR_H