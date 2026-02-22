#ifndef PROGRESS_TRACKER_H
#define PROGRESS_TRACKER_H
#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
class ProgressTracker : public QObject {
    Q_OBJECT
public:
    explicit ProgressTracker(QObject *parent = nullptr);
    ~ProgressTracker();
    void start();
    void updateProgress(qint64 bytesTransferred, qint64 totalBytes);
    void stop();
signals:
    void progressUpdated(double percentage, double mbPerSec, QString eta);
    void transferFinished();
private slots:
    void updateDisplay();
private:
    QElapsedTimer m_timer;
    qint64 m_totalBytes = 0;
    qint64 m_transferredBytes = 0;
    double m_mbPerSec = 0.0;
    QTimer *m_updateTimer;
    bool m_isRunning = false;
};
#endif // PROGRESS_TRACKER_H
