#include "ErrorManager.h"
#include <QDateTime>

ErrorManager::ErrorManager(QObject *parent) : QObject(parent) {}

ErrorManager::~ErrorManager() {}

void ErrorManager::logError(const QString& error) {
    QString fullError = QDateTime::currentDateTime().toString() + ": " + error;
    m_errors.append(fullError);
    emit errorAdded(fullError);
}

QStringList ErrorManager::errors() const {
    return m_errors;
}