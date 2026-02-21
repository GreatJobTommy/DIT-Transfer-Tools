/****************************************************************************
** DriveMonitor.h - Überwacht USB/Laufwerke für Reconnects
** Pollt Mounts, signalisiert Reconnects.
** Max 250 Zeilen.
****************************************************************************/

#ifndef DRIVEMONITOR_H
#define DRIVEMONITOR_H

#include &lt;QObject&gt;
#include &lt;QTimer&gt;
#include &lt;QSet&gt;
#include &lt;QStringList&gt;

class DriveMonitor : public QObject
{
    Q_OBJECT

public:
    explicit DriveMonitor(QObject *parent = nullptr);

signals:
    void driveReconnected(const QString &amp;drivePath);

private slots:
    void checkDrives();

private:
    QTimer *m_timer;
    QSet&lt;QString&gt; m_knownDrives;
    
    QStringList getMountedDrives() const;
    bool isDriveValid(const QString &amp;path) const;
};

#endif // DRIVEMONITOR_H