#ifndef NETWORKATTACHMENTFACADEVK_H
#define NETWORKATTACHMENTFACADEVK_H

#include "NetworkAttachmentFacadeInterface.h"

#include "NetworkFacadeVK.h"

#include "RelatedToAttachmentsProcessingVK.h"

#include "AttachmentJsonParserVK.h"

#include "AttachmentDefinerVK.h"

class NetworkAttachmentFacadeVK
        : public NetworkAttachmentFacadeInterface,
          public NetworkFacadeVK
{
public:
    constexpr static const uint8_t C_MAX_ATTACHMENT_UPLOADS_PER_TIME = 5;
    
    NetworkAttachmentFacadeVK(const std::shared_ptr<AttachmentJsonParserInterface> &attachmentParser,
                              const std::shared_ptr<NetworkRequestExecutorInterface> &executor);

    Error getAttachmentUrlById(const QString &attachmentIdString,
                               const RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag attachmentType,
                               QUrl &attachmentUrl);
    
    virtual Error downloadAttachment(const std::shared_ptr<AttachmentEntityBase> &attachmentToDownload,
                                     const RelatedToAttachmentsProcessingBase::EntityProcessingFlagType attachmentDownloadingFlag,
                                     std::unique_ptr<AttachmentContentBase> &downloadedAttachmentContent) override;
    

    virtual Error uploadDocument(const std::unique_ptr<AttachmentContentBase> &attachmentContentToUpload,
                                 const RelatedToEntityProcessingBase::EntityProcessingFlagType attachmentUploadingFlag,
                                 std::shared_ptr<AttachmentEntityBase> &uploadedAttachment) override;
    virtual Error uploadImage(const std::unique_ptr<AttachmentContentBase> &attachmentContentToUpload,
                              const RelatedToEntityProcessingBase::EntityProcessingFlagType attachmentUploadingFlag,
                              std::shared_ptr<AttachmentEntityBase> &uploadedAttachment) override;
};

#endif // NETWORKATTACHMENTFACADEVK_H
