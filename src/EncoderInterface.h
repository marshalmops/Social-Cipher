#ifndef ENCODERINTERFACE_H
#define ENCODERINTERFACE_H

#include "CipherKey.h"

class EncoderInterface
{
public:
    EncoderInterface         () = default;
    virtual ~EncoderInterface() = default;
    
    virtual bool generateKeys(CipherKey &privateKey,
                              CipherKey &publicKey) const = 0;
    
    virtual QByteArray encodeBytes(const QByteArray &bytes,
                                   const CipherKey &publicKey) const = 0;
    virtual QByteArray decodeBytes(const QByteArray &bytes,
                                   const CipherKey &privateKey) const = 0;
};

#endif // ENCODERINTERFACE_H
