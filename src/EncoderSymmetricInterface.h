#ifndef ENCODERSYMMETRICINTERFACE_H
#define ENCODERSYMMETRICINTERFACE_H

#include "CipherKey.h"

class EncoderSymmetricInterface
{
public:
    EncoderSymmetricInterface() = default;
    virtual ~EncoderSymmetricInterface() = default;
    
    virtual bool generateKey(CipherKey &key) const = 0;
    
    virtual QByteArray encodeBytes(const QByteArray &bytes,
                                   const CipherKey &key) const = 0;
    virtual QByteArray decodeBytes(const QByteArray &bytes,
                                   const CipherKey &key) const = 0;
};

#endif // ENCODERSYMMETRICINTERFACE_H
