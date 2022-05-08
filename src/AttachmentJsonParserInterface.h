#ifndef ATTACHMENTJSONPARSERINTERFACE_H
#define ATTACHMENTJSONPARSERINTERFACE_H

#include "EntityJsonParserInterface.h"
#include "AttachmentEntityBase.h"

#include "RelatedToAttachmentsProcessingBase.h"

class AttachmentJsonParserInterface : public EntityJsonParserInterface
{
public:
    AttachmentJsonParserInterface() = default;
    
    virtual bool jsonToAttachments (const QJsonValue &json,
                                    const RelatedToAttachmentsProcessingBase::EntityProcessingFlagType attachmentsProcessingFlag,
                                    std::vector<std::shared_ptr<AttachmentEntityBase>> &attachments) const = 0;
};

#endif // ATTACHMENTJSONPARSERINTERFACE_H
