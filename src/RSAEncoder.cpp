#include "RSAEncoder.h"

RSAEncoder::RSAEncoder()
{
    
}

bool RSAEncoder::generateKeys(CipherKey &privateKey,
                              CipherKey &publicKey) const
{
    QByteArray publicKeyBuffer, privateKeyBuffer;
    QRSAEncryption e;
    
    if (!e.generatePairKey(publicKeyBuffer, privateKeyBuffer, QRSAEncryption::Rsa::RSA_2048))
        return false;
    
    privateKey.setBytes(privateKeyBuffer);
    publicKey.setBytes(publicKeyBuffer);
    
    return true;
}

QByteArray RSAEncoder::encodeBytes(const QByteArray &bytes,
                                    const CipherKey &publicKey) const
{
    QRSAEncryption e;
    
    QByteArray encodedBytes = e.encode(bytes, publicKey.getBytes());
    
    return encodedBytes;
}

QByteArray RSAEncoder::decodeBytes(const QByteArray &bytes,
                                   const CipherKey &privateKey) const
{
    QRSAEncryption e;
    
    QByteArray decodedBytes = e.decode(bytes, privateKey.getBytes());

    return decodedBytes;
}
