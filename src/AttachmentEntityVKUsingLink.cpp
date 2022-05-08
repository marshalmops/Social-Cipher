#include "AttachmentEntityVKUsingLink.h"

AttachmentEntityVKUsingLink::AttachmentEntityVKUsingLink(const EntityId attachmentId, 
                                                         const EntityId peerId, 
                                                         const QUrl &link,
                                                         const QString &fileName)
    : AttachmentEntityVK{attachmentId, peerId},
      m_link{link},
      m_fileName(fileName.isEmpty() ? m_link.fileName() : fileName)
{
    
}

bool AttachmentEntityVKUsingLink::isValid() const
{
    if (m_link.isEmpty() || (m_link.fileName().isEmpty() && m_fileName.isEmpty())) return false;
    
    return true;
}

RelatedToAttachmentsProcessingVK::AttachmentOriginFlag AttachmentEntityVKUsingLink::getProcessingFlag() const
{
    return RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_STANDARD;
}

const QString AttachmentEntityVKUsingLink::getFileName()
{
    return m_fileName;
}

const QString AttachmentEntityVKUsingLink::getExtension()
{
    return AttachmentDefinerBase::getExtensionByFilename(m_fileName);
}

const QUrl AttachmentEntityVKUsingLink::getUrl()
{
    return m_link;
}
