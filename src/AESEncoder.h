#ifndef AESENCODER_H
#define AESENCODER_H

#include <QCryptographicHash>
#include <random>

#include "EncoderSymmetricInterface.h"

#include "qaesencryption.h"

class AESEncoder : public EncoderSymmetricInterface
{
public:
    AESEncoder();
    
    virtual bool generateKey(CipherKey &key) const override;
    
    virtual QByteArray encodeBytes(const QByteArray &bytes, 
                                   const CipherKey &key) const override;
    virtual QByteArray decodeBytes(const QByteArray &bytes, 
                                   const CipherKey &key) const override;
    
private:
    mutable QAESEncryption m_encryptor;
};

#endif // AESENCODER_H
