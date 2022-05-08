#include "AttachmentEntityBase.h"


AttachmentEntityBase::AttachmentEntityBase(const EntityId attachmentId)
    : m_attachmentId{attachmentId}
{
    
}

EntityInterface::EntityId AttachmentEntityBase::getAttachmentId() const
{
    return m_attachmentId;
}

bool AttachmentEntityBase::isValid() const
{
    return (m_attachmentId != 0);
}

