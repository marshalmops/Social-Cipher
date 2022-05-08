#include "AttachmentEntityVKUsingIdString.h"

AttachmentEntityVKUsingIdString::AttachmentEntityVKUsingIdString(const EntityId attachmentId, 
                                                         const EntityId peerId,
                                                         const QString &type)
    : AttachmentEntityVK{attachmentId, peerId},
      m_type{type}
{
    
}

bool AttachmentEntityVKUsingIdString::isValid() const
{
    if (m_type.isEmpty()) return false;
    
    return true;
}

RelatedToAttachmentsProcessingVK::AttachmentOriginFlag AttachmentEntityVKUsingIdString::getProcessingFlag() const
{
    return RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_EVENTIONAL;
}

QString AttachmentEntityVKUsingIdString::getType() const
{
    return m_type;
}

QString AttachmentEntityVKUsingIdString::getIdString() const
{
    return (/*m_type + */QString::number(m_peerId) + '_' + QString::number(m_attachmentId));
}
