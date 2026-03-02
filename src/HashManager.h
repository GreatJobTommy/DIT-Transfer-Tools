#ifndef HASHMANAGER_H
#define HASHMANAGER_H

#include <QString>
#include <QCryptographicHash>

class HashManager {
public:
    HashManager();
    QString computeSHA256(const QString& filePath);
};

#endif // HASHMANAGER_H