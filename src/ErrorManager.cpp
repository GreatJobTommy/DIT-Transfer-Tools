#include "ErrorManager.h"

ErrorManager::ErrorManager(QObject* parent)
    : QObject(parent) {}

ErrorManager::~ErrorManager() {}

void ErrorManager::addError(const QString& error) {
    m_errors.append(error);
    emit errorAdded(error);
}

QStringList ErrorManager::getErrors() const {
    return m_errors;
}

void ErrorManager::clearErrors() {
    m_errors.clear();
}