#ifndef ATTACHMENTDEFINERVK_H
#define ATTACHMENTDEFINERVK_H

#include "AttachmentDefinerBase.h"

#include "RelatedToAttachmentsProcessingVK.h"

class AttachmentDefinerVK
        : public AttachmentDefinerBase
{
public:
    AttachmentDefinerVK() = delete;
    
    static const QHash<QString, RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag>& getProcessingWayTypeStringHash();
    
    static RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag getProcessingWayByTypeString(const QString &typeString);
    static RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag getProcessingWayByExtension (const QString &fileName);
    
    static QString getTypeStringByProcessingWay(const RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag processingWay);
};

#endif // ATTACHMENTDEFINERVK_H
