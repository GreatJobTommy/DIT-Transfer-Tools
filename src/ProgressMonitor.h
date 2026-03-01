#ifndef PROGRESSMONITOR_H
#define PROGRESSMONITOR_H

#include <QObject>
#include <QElapsedTimer>
#include <QTextEdit>

class ProgressMonitor : public QObject {
    Q_OBJECT

public:
    explicit ProgressMonitor(QObject *parent = nullptr);
    ~ProgressMonitor();

    void startTransfer(qint64 totalBytes);
    void updateProgress(qint64 bytesTransferred);
    void finishTransfer();

    double currentSpeed() const; // MB/s
    qint64 eta() const; // seconds
    QString logs() const;

signals:
    void progressUpdated(double speed, qint64 eta, const QString& log);

private:
    QElapsedTimer m_timer;
    qint64 m_totalBytes;
    qint64 m_transferred;
    QStringList m_logs;
    QTextEdit* m_logWidget; // Optional, for display
};

#endif // PROGRESSMONITOR_H