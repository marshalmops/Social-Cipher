#ifndef READWRITEFILEINTERFACE_H
#define READWRITEFILEINTERFACE_H

#include <QString>
#include <QFile>
#include <memory>

class ReadWriteFileInterface
{
public:
    constexpr static const char C_ROWS_DIVIDER = '\n';
    
    ReadWriteFileInterface         (const QString &filename = QString());
    virtual ~ReadWriteFileInterface() = default;
    
    virtual bool fromFile()       = 0;
    virtual bool toFile  () const = 0;
    
protected:
    template<class T>
    bool numbFromChars(const char* chars, T &number) const;
    
    template<class T>
    bool numbToChars(const T numb, std::unique_ptr<char> &chars) const;
    
    QString m_filename;
};

#endif // READWRITEFILEINTERFACE_H
