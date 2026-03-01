#include "HashManager.h"
#include <QFile>
#include <QDebug>

HashManager::HashManager(QObject *parent) : QObject(parent), m_hash(QCryptographicHash::Sha256) {
    m_thread = new QThread(this);
    moveToThread(m_thread);
    m_thread->start();
}

HashManager::~HashManager() {
    m_thread->quit();
    m_thread->wait();
}

void HashManager::computeHash(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred("Cannot open file for hashing");
        return;
    }

    m_hash.reset();
    while (!file.atEnd()) {
        QByteArray chunk = file.read(8192);
        m_hash.addData(chunk);
    }
    file.close();

    m_resultHash = m_hash.result().toHex();
    emit hashComputed(m_resultHash);
}

QString HashManager::hash() const {
    return m_resultHash;
}

void HashManager::compute() {
    // Placeholder implementation
    qDebug() << "HashManager compute called";
}