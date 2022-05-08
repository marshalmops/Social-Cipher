#include "AttachmentLocalFileLink.h"

AttachmentLocalFileLink::AttachmentLocalFileLink(const QUrl &fileLink, 
                                                 const QMetaType::Type metaType)
    : m_localFileLink{fileLink},
      m_metaType     {metaType}
{
    
}

QUrl AttachmentLocalFileLink::getLocalFileLink() const
{
    return m_localFileLink;
}

QMetaType::Type AttachmentLocalFileLink::getMetaType() const
{
    return m_metaType;
}

QString AttachmentLocalFileLink::getLocalFilePathString() const
{
    return m_localFileLink.path();
}

bool AttachmentLocalFileLink::isValid() const
{
    return (!m_localFileLink.isEmpty() && m_metaType != QMetaType::Type::UnknownType
            ? true : false);
}
