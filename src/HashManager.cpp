#include "HashManager.h"
#include <QDebug>

QByteArray HashManager::computeHash(const QString &filePath, QCryptographicHash::Algorithm algo) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << filePath;
        return QByteArray();
    }
    QCryptographicHash hash(algo);
    if (!hash.addData(&file)) {
        return QByteArray();
    }
    return hash.result();
}

bool HashManager::verifyHash(const QString &filePath, const QByteArray &expectedHash) {
    QByteArray actual = computeHash(filePath);
    return actual == expectedHash;
}
