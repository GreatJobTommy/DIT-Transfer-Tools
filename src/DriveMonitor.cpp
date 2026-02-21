/****************************************************************************
** DriveMonitor.cpp - Laufwerk-Überwachung via df/mount Polling
****************************************************************************/

#include &quot;DriveMonitor.h&quot;
#include &lt;QProcess&gt;
#include &lt;QDir&gt;
#include &lt;QDebug&gt;
#include &lt;QStandardPaths&gt;

DriveMonitor::DriveMonitor(QObject *parent)
    : QObject(parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, &amp;QTimer::timeout, this, &amp;DriveMonitor::checkDrives);
    m_timer-&gt;start(5000); // 5s Poll
    checkDrives(); // Initial
}

QStringList DriveMonitor::getMountedDrives() const
{
    QProcess process;
    process.start(&quot;df&quot;, { &quot;-hT&quot;, &quot;/media&quot; });
    process.waitForFinished(2000);
    
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split(&apos;\n&apos;, Qt::SkipEmptyParts);
    
    QStringList drives;
    for (const QString &amp;line : lines) {
        if (line.contains(&quot;/media&quot;) &amp;&amp; !line.contains(&quot;tmpfs&quot;)) {
            QStringList parts = line.split(QRegExp(&quot;\\s+&quot;), Qt::SkipEmptyParts);
            if (parts.size() &gt; 5) {
                drives &lt;&lt; parts[5].trimmed();
            }
        }
    }
    return drives;
}

void DriveMonitor::checkDrives()
{
    QStringList current = getMountedDrives();
    
    for (const QString &amp;drive : current) {
        if (!m_knownDrives.contains(drive)) {
            if (isDriveValid(drive)) {
                m_knownDrives.insert(drive);
                emit driveReconnected(drive);
            }
        }
    }
    
    // Bekannte entfernen wenn weg (optional)
}

bool DriveMonitor::isDriveValid(const QString &amp;path) const
{
    QDir dir(path);
    return dir.exists() &amp;&amp; !dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot).isEmpty();
}

// EOF
// Hinweis: Linux-spezifisch (df). Für Win: wmic logicaldisk