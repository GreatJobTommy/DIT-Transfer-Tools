/****************************************************************************
** HashManager.h - Live SHA256 Hashing für Transfers
** Thread-sicher, Streaming-Hash-Update.
** Max 250 Zeilen.
****************************************************************************/

#ifndef HASHMANAGER_H
#define HASHMANAGER_H

#include &lt;QObject&gt;
#include &lt;QByteArray&gt;
#include &lt;QCryptographicHash&gt;

class HashManager : public QObject
{
    Q_OBJECT

public:
    explicit HashManager(QObject *parent = nullptr);
    
    void update(const QByteArray &amp;data);
    QString finalHash() const;
    void reset();

signals:
    void hashReady(const QByteArray &amp;partialHash);

private:
    QCryptographicHash m_hashCtx{QCryptographicHash::Sha256};
};

#endif // HASHMANAGER_H