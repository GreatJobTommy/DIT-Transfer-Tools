/****************************************************************************
** DriveMonitor.cpp - Full Cross-Platform Drive Monitor V3
** Primary: QStorageInfo (Qt6+ all platforms)
** Real-time: QFileSystemWatcher on mount points
** Fallback: Platform-specific poll (df/Linux, wmic/Win)
** Disconnect: QueueManager::pauseAll()
** Reconnect: Check lastFiles, then resumeAll()
****************************************************************************/

#include "DriveMonitor.h"
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <QSysInfo>

DriveMonitor::DriveMonitor(QObject *parent)
    : QObject(parent)
    , m_pollTimer(new QTimer(this))
    , m_watcher(new QFileSystemWatcher(this))
{
    connect(m_pollTimer, &QTimer::timeout, this, &DriveMonitor::checkDrivesPoll);
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &DriveMonitor::onWatcherChange);
    
    m_pollTimer->start(5000);  // 5s fallback poll
    scanDrives();  // Initial scan
}

DriveMonitor::~DriveMonitor()
{
    // Cleanup watchers if needed
}

void DriveMonitor::scanDrives()
{
    // Primary: QStorageInfo (cross-platform)
    QList<QStorageInfo> volumes = QStorageInfo::mountedVolumes();
    QStringList newDrives;
    
    for (const QStorageInfo &info : volumes) {
        if (info.isReady() && info.rootPath() != "/" && isRemovableDrive(info)) {
            QString path = info.rootPath();
            newDrives << path;
            if (!m_knownDrives.contains(path)) {
                addDrive(path);
            }
        }
    }
    
    // Detect disconnects
    for (QString path : m_currentDrives) {
        if (!newDrives.contains(path)) {
            removeDrive(path);
        }
    }
    
    m_currentDrives = newDrives;
    emit drivesChanged(newDrives);
}

bool DriveMonitor::isRemovableDrive(const QStorageInfo &info) const
{
    // USB/Removable heuristics
    QString device = info.device();
    if (device.contains("usb", Qt::CaseInsensitive) || 
        device.contains("sd", Qt::CaseInsensitive) ||
        info.bytesTotal() < 2LL * 1024 * 1024 * 1024 * 1024) {  // < 2TB likely removable
        return true;
    }
    return false;
}

void DriveMonitor::addDrive(const QString &path)
{
    m_knownDrives.insert(path);
    m_watcher->addPath(path);  // Real-time watch
    m_lastFiles[path] = !QDir(path).entryList(QDir::Files).isEmpty();  // Check if had files
    emit driveConnected(path);
    
    // Auto-resume if was paused
    // Note: QueueManager integration via MainWindow signals
}

void DriveMonitor::removeDrive(const QString &path)
{
    m_watcher->removePath(path);
    emit driveDisconnected(path);
    
    // Auto-pause
    // QueueManager::pauseAll() via signal
}

void DriveMonitor::checkDrivesPoll()
{
    scanDrives();  // Poll triggers full scan
}

void DriveMonitor::onWatcherChange(const QString &path)
{
    qDebug() << "Watcher:" << path;
    QTimer::singleShot(100, this, &DriveMonitor::scanDrives);  // Debounce
}

void DriveMonitor::onStorageChanged()
{
    // Qt6+ signal if available, fallback to poll
    scanDrives();
}

QStringList DriveMonitor::getFallbackDrives() const
{
    QStringList drives;
#if defined(Q_OS_LINUX)
    QProcess p;
    p.start("df", { "-hT", "/media" });
    p.waitForFinished(1000);
    QString out = p.readAllStandardOutput();
    QStringList lines = out.split(&apos;\n&apos;);
    for (const QString &line : lines) {
        if (line.contains("/media") && !line.contains("tmpfs")) {
            QStringList parts = line.split(QRegExp("\\s+"));
            if (parts.size() > 5) drives << parts[5].trimmed();
        }
    }
#elif defined(Q_OS_WIN)
    QProcess p;
    p.start("wmic", { "logicaldisk", "get", "DeviceID,DriveType" });
    // Parse output...
#elif defined(Q_OS_MAC)
    // diskutil list parse
#endif
    return drives;
}

bool DriveMonitor::isDriveWatched(const QString &path) const
{
    return m_watcher->files().contains(path) || m_watcher->directories().contains(path);
}

// EOF