/****************************************************************************
** HashManager.cpp - SHA256 Implementation
****************************************************************************/

#include &quot;HashManager.h&quot;
#include &lt;QThread&gt;
#include &lt;QDebug&gt;

HashManager::HashManager(QObject *parent)
    : QObject(parent)
{
}

void HashManager::update(const QByteArray &amp;data)
{
    m_hashCtx.addData(data);
    emit hashReady(m_hashCtx.hash().toHex().left(16)); // Partial 16 chars
}

QString HashManager::finalHash() const
{
    return QString::fromLatin1(m_hashCtx.result().toHex());
}

void HashManager::reset()
{
    m_hashCtx = QCryptographicHash(QCryptographicHash::Sha256);
}

// EOF