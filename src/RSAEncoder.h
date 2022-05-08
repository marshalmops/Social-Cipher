#ifndef RSAENCODER_H
#define RSAENCODER_H

#include <QString>
#include <QByteArray>

#include <qrsaencryption.h>

#include "EncoderInterface.h"

class RSAEncoder : public EncoderInterface
{
public:
    RSAEncoder();
    
    virtual bool generateKeys(CipherKey &privateKey,
                              CipherKey &publicKey) const override;
    
    virtual QByteArray encodeBytes(const QByteArray &bytes,
                                   const CipherKey &publicKey) const override;
    virtual QByteArray decodeBytes(const QByteArray &bytes,
                                   const CipherKey &privateKey) const override;
};

#endif // RSAENCODER_H
