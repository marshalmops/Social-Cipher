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

QByteArray RSAEncoder::encodeString(const QString &string,
                                    const CipherKey &publicKey) const
{
    QRSAEncryption e;
    
    QByteArray encodedString = e.encode(string.toUtf8(), publicKey.getBytes());
    
    return encodedString;
}

QString RSAEncoder::decodeString(const QByteArray &bytes,
                                 const CipherKey &privateKey) const
{
    QRSAEncryption e;
    
    QString decodedString = QString::fromUtf8(e.decode(bytes, privateKey.getBytes()));

    return decodedString;
}
