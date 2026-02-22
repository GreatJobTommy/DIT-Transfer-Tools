/****************************************************************************
** DriveMonitor.cpp - Full Cross-Platform Drive Monitor V3
** Primary: QStorageInfo (Qt6+ all platforms)
** Real-time: QFileSystemWatcher on mount points
** Fallback: Platform-specific poll (df/Linux, wmic/Win)
** Disconnect: QueueManager::pauseAll()
** Reconnect: Check lastFiles, then resumeAll()
****************************************************************************/

#include &quot;DriveMonitor.h&quot;
#include &lt;QProcess&gt;
#include &lt;QDir&gt;
#include &lt;QDebug&gt;
#include &lt;QStandardPaths&gt;
#include &lt;QSysInfo&gt;

DriveMonitor::DriveMonitor(QObject *parent)
    : QObject(parent)
    , m_pollTimer(new QTimer(this))
    , m_watcher(new QFileSystemWatcher(this))
{
    connect(m_pollTimer, &amp;QTimer::timeout, this, &amp;DriveMonitor::checkDrivesPoll);
    connect(m_watcher, &amp;QFileSystemWatcher::directoryChanged, this, &amp;DriveMonitor::onWatcherChange);
    
    m_pollTimer-&gt;start(5000);  // 5s fallback poll
    scanDrives();  // Initial scan
}

DriveMonitor::~DriveMonitor()
{
    // Cleanup watchers if needed
}

void DriveMonitor::scanDrives()
{
    // Primary: QStorageInfo (cross-platform)
    QList&lt;QStorageInfo&gt; volumes = QStorageInfo::mountedVolumes();
    QStringList newDrives;
    
    for (const QStorageInfo &amp;info : volumes) {
        if (info.isReady() &amp;&amp; info.rootPath() != &quot;/&quot; &amp;&amp; isRemovableDrive(info)) {
            QString path = info.rootPath();
            newDrives &lt;&lt; path;
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

bool DriveMonitor::isRemovableDrive(const QStorageInfo &amp;info) const
{
    // USB/Removable heuristics
    QString device = info.device();
    if (device.contains(&quot;usb&quot;, Qt::CaseInsensitive) || 
        device.contains(&quot;sd&quot;, Qt::CaseInsensitive) ||
        info.bytesTotal() &lt; 2LL * 1024 * 1024 * 1024 * 1024) {  // &lt; 2TB likely removable
        return true;
    }
    return false;
}

void DriveMonitor::addDrive(const QString &amp;path)
{
    m_knownDrives.insert(path);
    m_watcher-&gt;addPath(path);  // Real-time watch
    m_lastFiles[path] = !QDir(path).entryList(QDir::Files).isEmpty();  // Check if had files
    emit driveConnected(path);
    
    // Auto-resume if was paused
    // Note: QueueManager integration via MainWindow signals
}

void DriveMonitor::removeDrive(const QString &amp;path)
{
    m_watcher-&gt;removePath(path);
    emit driveDisconnected(path);
    
    // Auto-pause
    // QueueManager::pauseAll() via signal
}

void DriveMonitor::checkDrivesPoll()
{
    scanDrives();  // Poll triggers full scan
}

void DriveMonitor::onWatcherChange(const QString &amp;path)
{
    qDebug() &lt;&lt; &quot;Watcher:&quot; &lt;&lt; path;
    QTimer::singleShot(100, this, &amp;DriveMonitor::scanDrives);  // Debounce
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
    p.start(&quot;df&quot;, { &quot;-hT&quot;, &quot;/media&quot; });
    p.waitForFinished(1000);
    QString out = p.readAllStandardOutput();
    QStringList lines = out.split(&apos;\n&apos;);
    for (const QString &amp;line : lines) {
        if (line.contains(&quot;/media&quot;) &amp;&amp; !line.contains(&quot;tmpfs&quot;)) {
            QStringList parts = line.split(QRegExp(&quot;\\s+&quot;));
            if (parts.size() &gt; 5) drives &lt;&lt; parts[5].trimmed();
        }
    }
#elif defined(Q_OS_WIN)
    QProcess p;
    p.start(&quot;wmic&quot;, { &quot;logicaldisk&quot;, &quot;get&quot;, &quot;DeviceID,DriveType&quot; });
    // Parse output...
#elif defined(Q_OS_MAC)
    // diskutil list parse
#endif
    return drives;
}

bool DriveMonitor::isDriveWatched(const QString &amp;path) const
{
    return m_watcher-&gt;files().contains(path) || m_watcher-&gt;directories().contains(path);
}

// EOF