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
    
   
    virtual QByteArray encodeString(const QString &string,
                                    const CipherKey &publicKey) const override;
    virtual QString decodeString(const QByteArray &bytes,
                                 const CipherKey &privateKey) const override;
};

#endif // RSAENCODER_H
