#ifndef RELATEDTOATTACHMENTSPROCESSINGVK_H
#define RELATEDTOATTACHMENTSPROCESSINGVK_H

#include "RelatedToAttachmentsProcessingBase.h"

class RelatedToAttachmentsProcessingVK
        : public RelatedToAttachmentsProcessingBase
{
public:
    enum AttachmentOriginFlag : RelatedToEntityProcessingBase::EntityProcessingFlagType {
        AOF_INVALID = 0,
        AOF_EVENTIONAL = 1,
        AOF_STANDARD = 2,
        AOF_UPLOADED = 4
    };
    
    enum AttachmentProcessingWayFlag : RelatedToEntityProcessingBase::EntityProcessingFlagType {
        AUF_INVALID = 256,
        AUF_PHOTO_PROCESSING = 512,
        AUF_DOCUMENT_PROCESSING = 1024
    };
    
    RelatedToAttachmentsProcessingVK() = delete;
};

#endif // RELATEDTOATTACHMENTSPROCESSINGVK_H
