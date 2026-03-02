#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include <QObject>
#include <QStringList>
#include <QMutex>

class LogsDockWidget;

class ErrorManager : public QObject {
    Q_OBJECT

public:
    enum LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        CRITICAL = 4
    };

    explicit ErrorManager(QObject* parent = nullptr);
    ~ErrorManager();

    void addError(const QString& error, const QString& destination = QString());
    void addLog(const QString& message, LogLevel level = INFO, const QString& destination = QString());
    QStringList getErrors() const;
    void clearErrors();
    void setLogsWidget(LogsDockWidget* widget);
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;

signals:
    void errorAdded(const QString& error);
    void logAdded(const QString& message, LogLevel level, const QString& destination);

private:
    QStringList m_errors;
    LogsDockWidget* m_logsWidget;
    LogLevel m_logLevel;
    QMutex m_mutex;
};

#endif // ERRORMANAGER_H