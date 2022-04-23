#include "CipherKey.h"

CipherKey::CipherKey(const QByteArray &bytes)
    : m_bytes{bytes}
{
    
}

QByteArray CipherKey::getBytes() const
{
    return m_bytes;
}

bool CipherKey::setBytes(const QByteArray &bytes)
{
    if (bytes.isEmpty()) return false;
    
    m_bytes = bytes;
    
    return true;
}

void CipherKey::resetBytes()
{
    m_bytes.clear();
}
