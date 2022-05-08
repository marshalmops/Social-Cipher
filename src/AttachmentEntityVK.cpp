#include "AttachmentEntityVK.h"

AttachmentEntityVK::AttachmentEntityVK(const EntityId attachmentId, 
                                       const EntityId peerId)
    : AttachmentEntityBase{attachmentId},
      m_peerId{peerId}
{
    
}

EntityInterface::EntityId AttachmentEntityVK::getPeerId() const
{
    return m_peerId;
}
