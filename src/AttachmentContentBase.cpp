#include "AttachmentContentBase.h"
#include "AttachmentManagerBase.h"

AttachmentContentBase::AttachmentContentBase(const QString &fileName,
                                             const QByteArray &bytes)
    : m_bytes   {bytes},
      m_fileName{fileName}
{
    
}

const QByteArray &AttachmentContentBase::getByteArray() const
{
    return m_bytes;
}

const QString &AttachmentContentBase::getFileName() const
{
    return m_fileName;
}

//const QString &AttachmentContentBase::getExtension() const
//{
//    return m_extension;
//}

bool AttachmentContentBase::isValid() const
{
    return (!m_bytes.isEmpty() && !m_fileName.isEmpty() ? true : false);
}
