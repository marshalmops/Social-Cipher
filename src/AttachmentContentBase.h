#ifndef ATTACHMENTCONTENTBASE_H
#define ATTACHMENTCONTENTBASE_H

#include <QByteArray>
#include <QMetaType>

class AttachmentManagerBase;

class AttachmentContentBase
{
public:
    AttachmentContentBase() = default;
    AttachmentContentBase(const QString &fileName,
                          const QByteArray &bytes);
    
    const QByteArray& getByteArray() const;
    const QString&    getFileName() const;
    
    bool isValid() const;
    
    friend AttachmentManagerBase;
    
protected:
    QByteArray m_bytes;
    QString    m_fileName;
};

#endif // ATTACHMENTCONTENTBASE_H
