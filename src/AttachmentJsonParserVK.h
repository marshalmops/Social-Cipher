#ifndef ATTACHMENTJSONPARSERVK_H
#define ATTACHMENTJSONPARSERVK_H

#include "AttachmentJsonParserInterface.h"

#include "AttachmentEntityVKUsingIdString.h"
#include "AttachmentEntityVKUsingLink.h"

#include "AttachmentDefinerVK.h"

#include "RelatedToAttachmentsProcessingVK.h"

class AttachmentJsonParserVK : public AttachmentJsonParserInterface
{
public:
    constexpr static const char* C_ATTACHMENT_TYPE_FIELD_NAME         = "type";
    constexpr static const char* C_ATTACHMENT_URL_FIELD_NAME          = "url";
    constexpr static const char* C_ATTACHMENT_ID_FIELD_NAME           = "id";
    constexpr static const char* C_ATTACHMENT_OWNER_FIELD_NAME        = "owner_id";
    constexpr static const char* C_ATTACHMENT_DOC_FILENAME_FIELD_NAME = "title";
    
    AttachmentJsonParserVK();
    
    virtual bool jsonToAttachments(const QJsonValue &json,
                                   const RelatedToAttachmentsProcessingBase::EntityProcessingFlagType attachmentsProcessingFlag,
                                   std::vector<std::shared_ptr<AttachmentEntityBase>> &attachments) const override;

protected:
    bool jsonEventsToAttachments(const QJsonValue &json,
                                 std::vector<std::shared_ptr<AttachmentEntityBase>> &attachments) const;
    bool jsonStandardToAttachments(const QJsonValue &json,
                                   std::vector<std::shared_ptr<AttachmentEntityBase>> &attachments) const;
    bool jsonUploadedToAttachments(const QJsonValue &json,
                                   const RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag attachmentTypeFlag,
                                   std::vector<std::shared_ptr<AttachmentEntityBase>> &attachments) const;
};

#endif // ATTACHMENTJSONPARSERVK_H
