#ifndef NETWORKATTACHMENTFACADEINTERFACE_H
#define NETWORKATTACHMENTFACADEINTERFACE_H

#include <QUrlQuery>
#include <QHttpMultiPart>

#include "NetworkFacadeInterface.h"

#include "AttachmentContentBase.h"
#include "AttachmentEntityBase.h"

#include "AttachmentJsonParserInterface.h"

#include "RelatedToAttachmentsProcessingBase.h"

#include "AttachmentDefinerBase.h"

class NetworkAttachmentFacadeInterface : public NetworkFacadeInterface
{
public:
    NetworkAttachmentFacadeInterface(const std::shared_ptr<AttachmentJsonParserInterface> &attachmentParser,
                                     const std::shared_ptr<NetworkRequestExecutorInterface> &executor);

    virtual Error uploadDocument(const std::unique_ptr<AttachmentContentBase> &attachmentContentToUpload,
                                 const RelatedToAttachmentsProcessingBase::EntityProcessingFlagType attachmentUploadingFlag,
                                 std::shared_ptr<AttachmentEntityBase> &uploadedAttachment) = 0;
    virtual Error uploadImage   (const std::unique_ptr<AttachmentContentBase> &attachmentContentToUpload,
                                 const RelatedToAttachmentsProcessingBase::EntityProcessingFlagType attachmentUploadingFlag,
                                 std::shared_ptr<AttachmentEntityBase> &uploadedAttachment) = 0;
    
    virtual Error downloadAttachment(const std::shared_ptr<AttachmentEntityBase> &attachmentToDownload,
                                     const RelatedToAttachmentsProcessingBase::EntityProcessingFlagType attachmentDownloadingFlag,
                                     std::unique_ptr<AttachmentContentBase> &downloadedAttachmentContent) = 0;
    
    virtual bool initialize() override;
    
protected:
    std::shared_ptr<AttachmentJsonParserInterface> m_attachmentParser;
};

#endif // NETWORKATTACHMENTFACADEINTERFACE_H
