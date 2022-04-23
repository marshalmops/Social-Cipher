#include "ReadWriteFileInterface.h"

ReadWriteFileInterface::ReadWriteFileInterface(const QString &filename)
    : m_filename(filename)
{
    
}

template<class T>
bool ReadWriteFileInterface::numbFromChars(const char *chars, T &number) const
{
    auto socialNetworkTypeLength {sizeof(T)};
    
    if (strlen(chars) > socialNetworkTypeLength)
        return false;
    
    uint32_t currentByteShift{0};
    
    for (auto i = 0; i < strlen(chars); ++i) {
        number |= (chars[i] << currentByteShift);
        
        currentByteShift += 8;
    }
    
    return true;
}

template<class T>
bool ReadWriteFileInterface::numbToChars(const T numb, std::unique_ptr<char> &chars) const
{
    auto numbBytesCount {sizeof(T)};
    
    chars = std::unique_ptr<char>(new char[numbBytesCount + 1]);
    
    uint64_t currentBitMask  {0};
    uint32_t currentByteShift{0};
    
    for (auto i = 0; i < numbBytesCount; ++i) {
        *(chars.get() + i) = ((numb & currentBitMask) >> currentByteShift);
        
        currentBitMask  <<= 8;
        currentByteShift += 8;
    }
    
    std::reverse(chars.get(), chars.get() + numbBytesCount);
    
    *(chars.get() + numbBytesCount) = '\0';
    
    return true;
}
