#ifndef ATTACHMENTENTITYBASE_H
#define ATTACHMENTENTITYBASE_H

#include "EntityInterface.h"

#include "AttachmentDefinerBase.h"

class AttachmentEntityBase : public EntityInterface
{
public:
    AttachmentEntityBase() = default;
    AttachmentEntityBase(const AttachmentEntityBase &) = default;
    AttachmentEntityBase(const EntityId attachmentId);
    
    EntityId getAttachmentId() const;
    
    virtual bool isValid() const override;
    
protected:
    EntityId m_attachmentId;
};

#endif // ATTACHMENTENTITYBASE_H
