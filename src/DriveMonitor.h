/****************************************************************************
** DriveMonitor.h - Überwacht USB/Laufwerke cross-platform V3
** QStorageInfo + QFileSystemWatcher + df-poll fallback
** Signals: connected/disconnected, drivesChanged
** Max 250 Zeilen.
****************************************************************************/

#ifndef DRIVEMONITOR_H
#define DRIVEMONITOR_H

#include &lt;QObject&gt;
#include &lt;QTimer&gt;
#include &lt;QSet&gt;
#include &lt;QMap&gt;
#include &lt;QFileSystemWatcher&gt;
#include &lt;QStorageInfo&gt;
#include &lt;QStringList&gt;
#include "ErrorManager.h"

class DriveMonitor : public QObject
{
    Q_OBJECT

public:
    explicit DriveMonitor(QObject *parent = nullptr);
    ~DriveMonitor();

    QStringList getCurrentDrives() const { return m_currentDrives; }
    bool isDriveWatched(const QString &amp;path) const;

signals:
    void driveConnected(const QString &amp;drivePath);
    void driveDisconnected(const QString &amp;drivePath);
    void drivesChanged(const QStringList &amp;availableDrives);

private slots:
    void checkDrivesPoll();
    void onStorageChanged();
    void onWatcherChange(const QString &amp;path);

private:
    void scanDrives();
    bool isRemovableDrive(const QStorageInfo &amp;info) const;
    void addDrive(const QString &amp;path);
    void removeDrive(const QString &amp;path);
    QStringList getFallbackDrives() const;  // df/wmic fallback

    QTimer *m_pollTimer;
    QFileSystemWatcher *m_watcher;
    QSet&lt;QString&gt; m_knownDrives;
    QStringList m_currentDrives;
    QMap&lt;QString, bool&gt; m_lastFiles;  // For resume check: path -&gt; had files
};

#endif // DRIVEMONITOR_H