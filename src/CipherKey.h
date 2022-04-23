#ifndef CIPHERKEY_H
#define CIPHERKEY_H

#include <QByteArray>

class CipherKey
{
public:
    CipherKey(const QByteArray &bytes = QByteArray());
    
    QByteArray getBytes() const;
    bool       setBytes(const QByteArray &bytes);
    
    void resetBytes();
    
private:
    QByteArray m_bytes;
};

#endif // CIPHERKEY_H
