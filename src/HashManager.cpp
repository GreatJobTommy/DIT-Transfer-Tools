#include "HashManager.h"
#include <QFile>
#include <QDebug>

HashManager::HashManager() {}

QString HashManager::computeSHA256(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << filePath;
        return QString();
    }
    QCryptographicHash hash(QCryptographicHash::Sha256);
    if (hash.addData(&file)) {
        return hash.result().toHex();
    }
    return QString();
}