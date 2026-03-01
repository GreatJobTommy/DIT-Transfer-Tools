#ifndef DRIVEMONITOR_H
#define DRIVEMONITOR_H

#include <QObject>
#include <QTimer>
#include <QStorageInfo>

class DriveMonitor : public QObject {
    Q_OBJECT

public:
    explicit DriveMonitor(QObject *parent = nullptr);
    ~DriveMonitor();

    QList<QStorageInfo> availableDrives() const;

signals:
    void driveAdded(const QStorageInfo& drive);
    void driveRemoved(const QStorageInfo& drive);

private slots:
    void checkDrives();

private:
    QList<QStorageInfo> m_previousDrives;
    QTimer* m_timer;
};

#endif // DRIVEMONITOR_H