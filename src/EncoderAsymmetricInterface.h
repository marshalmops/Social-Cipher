#ifndef ENCODERASYMMETRICINTERFACE_H
#define ENCODERASYMMETRICINTERFACE_H

#include "CipherKey.h"

class EncoderAsymmetricInterface
{
public:
    EncoderAsymmetricInterface         () = default;
    virtual ~EncoderAsymmetricInterface() = default;
    
    virtual bool generateKeys(CipherKey &privateKey,
                              CipherKey &publicKey) const = 0;
    
    virtual QByteArray encodeBytes(const QByteArray &bytes,
                                   const CipherKey &publicKey) const = 0;
    virtual QByteArray decodeBytes(const QByteArray &bytes,
                                   const CipherKey &privateKey) const = 0;
};

#endif // ENCODERASYMMETRICINTERFACE_H
