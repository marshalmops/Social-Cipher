#ifndef ATTACHMENTMANAGERVK_H
#define ATTACHMENTMANAGERVK_H

#include "AttachmentManagerBase.h"

#include "AttachmentEntityVKUsingIdString.h"
#include "AttachmentEntityVKUsingLink.h"

#include "NetworkAttachmentFacadeVK.h"

#include "AttachmentDefinerVK.h"

class AttachmentManagerVK : public AttachmentManagerBase
{
public:
    AttachmentManagerVK(std::unique_ptr<NetworkAttachmentFacadeInterface> &attachmentsFacade);
    
//    virtual Error processNewAttachments(const std::vector<std::unique_ptr<AttachmentEntityBase>> &attachmentsToProcess,
//                                        std::vector<std::unique_ptr<AttachmentLocalFileLink>> &attachmentsLocalFileLinks) override;
    
    virtual Error uploadNewAttachment  (const std::unique_ptr<AttachmentContentBase> &attachmentContentToUpload,
                                        std::shared_ptr<AttachmentEntityBase>        &uploadedAttachment) override;
    virtual Error downloadNewAttachment(const std::shared_ptr<AttachmentEntityBase> &attachmentToDownload,
                                        std::unique_ptr<AttachmentContentBase> &downloadedAttachment) override;
    
private:
//    Error processAttachmentFromEvent   (std::unique_ptr<AttachmentEntityVK> &attachment);
//    Error processAttachmentFromStandard(std::unique_ptr<AttachmentEntityVK> &attachment);
    
    NetworkAttachmentFacadeVK* getAttachmentFacadeVK() const;
};

#endif // ATTACHMENTMANAGERVK_H
