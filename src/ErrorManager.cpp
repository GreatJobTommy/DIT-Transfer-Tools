#include "ErrorManager.h"
#include "LogsDockWidget.h"

ErrorManager::ErrorManager(QObject* parent)
    : QObject(parent), m_logsWidget(nullptr), m_logLevel(INFO) {}

ErrorManager::~ErrorManager() {}

void ErrorManager::addError(const QString& error, const QString& destination) {
    QMutexLocker locker(&m_mutex);
    m_errors.append(error);
    addLog(error, ERROR, destination);
    emit errorAdded(error);
}

void ErrorManager::addLog(const QString& message, LogLevel level, const QString& destination) {
    QMutexLocker locker(&m_mutex);
    if (level >= m_logLevel) {
        if (m_logsWidget) {
            m_logsWidget->appendLog(message, static_cast<LogsDockWidget::LogLevel>(level), destination);
        }
        emit logAdded(message, level, destination);
    }
}

QStringList ErrorManager::getErrors() const {
    QMutexLocker locker(&m_mutex);
    return m_errors;
}

void ErrorManager::clearErrors() {
    QMutexLocker locker(&m_mutex);
    m_errors.clear();
}

void ErrorManager::setLogsWidget(LogsDockWidget* widget) {
    QMutexLocker locker(&m_mutex);
    m_logsWidget = widget;
}

void ErrorManager::setLogLevel(LogLevel level) {
    QMutexLocker locker(&m_mutex);
    m_logLevel = level;
    if (m_logsWidget) {
        m_logsWidget->setLogLevel(static_cast<LogsDockWidget::LogLevel>(level));
    }
}

ErrorManager::LogLevel ErrorManager::getLogLevel() const {
    QMutexLocker locker(&m_mutex);
    return m_logLevel;
}