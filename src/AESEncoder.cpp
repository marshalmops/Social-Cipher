#include "AESEncoder.h"

AESEncoder::AESEncoder()
    : m_encryptor{QAESEncryption::Aes::AES_256, QAESEncryption::Mode::ECB, QAESEncryption::Padding::ZERO}
{
    
}

bool AESEncoder::generateKey(CipherKey &key) const
{
    std::random_device randDevice{};
    std::mt19937_64 randGenerator{randDevice()};
    
    uint64_t   keySeed      {randGenerator()};
    QByteArray randNumbBytes{};
    
    randNumbBytes.setNum(keySeed);
    
    QByteArray keyBytes{QCryptographicHash::hash(randNumbBytes, QCryptographicHash::Algorithm::Sha256)};
    
    if (keyBytes.isEmpty()) return false;
    
    key = std::move(keyBytes);
    
    return true;
}

QByteArray AESEncoder::encodeBytes(const QByteArray &bytes, 
                                   const CipherKey &key) const
{
    if (key.getBytes().isEmpty()) return bytes;
    
    QByteArray encodedBytes{m_encryptor.encode(bytes, key.getBytes())};
    
    return encodedBytes;
}

QByteArray AESEncoder::decodeBytes(const QByteArray &bytes, 
                                   const CipherKey &key) const
{
    if (key.getBytes().isEmpty()) return bytes;
    
    QByteArray decodedBytes{m_encryptor.decode(bytes, key.getBytes())};
    
    return decodedBytes;
}
