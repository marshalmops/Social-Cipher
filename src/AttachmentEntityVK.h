#ifndef ATTACHMENTENTITYVK_H
#define ATTACHMENTENTITYVK_H

#include "AttachmentEntityBase.h"
#include "RelatedToAttachmentsProcessingVK.h"

class AttachmentEntityVK : public AttachmentEntityBase
{
public:
    AttachmentEntityVK() = default;
    AttachmentEntityVK(const EntityId attachmentId,
                       const EntityId peerId);
    
    EntityId getPeerId() const;
    
    virtual RelatedToAttachmentsProcessingVK::AttachmentOriginFlag getProcessingFlag() const = 0;
    
protected:
    EntityId m_peerId;
};

#endif // ATTACHMENTENTITYVK_H
