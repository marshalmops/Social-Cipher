#ifndef ATTACHMENTMANAGERBASE_H
#define ATTACHMENTMANAGERBASE_H

#include <QFile>

#include "Error.h"

#include "AttachmentDefinerBase.h"
#include "AttachmentContentBase.h"
#include "AttachmentLocalFileLink.h"
#include "AttachmentEntityBase.h"

#include "NetworkAttachmentFacadeInterface.h"

#include "AttachmentDefinerBase.h"

// TODO: IMPLEMENT DEALLOCATING AND ALLOCATING OF ATTACHMENTS' CONTENT

class AttachmentManagerBase
{
public:
    using AttachmentsLocalFilesHash = QHash<AttachmentEntityBase::EntityId, AttachmentLocalFileLink>;
    
    AttachmentManagerBase(std::unique_ptr<NetworkAttachmentFacadeInterface> &attachmentsFacade);
    
//    virtual Error processNewAttachments(const std::vector<std::unique_ptr<AttachmentEntityBase>> &attachmentsToProcess,
//                                        std::vector<std::unique_ptr<AttachmentLocalFileLink>> &attachmentsLocalFileLinks) = 0;
    virtual Error downloadNewAttachment(const std::shared_ptr<AttachmentEntityBase> &attachmentToDownload,
                                        std::unique_ptr<AttachmentContentBase> &downloadedAttachment) = 0;
    
    virtual Error uploadNewAttachment(const std::unique_ptr<AttachmentContentBase> &attachmentContentToUpload,
                                      std::shared_ptr<AttachmentEntityBase> &uploadedAttachment) = 0;
    
    Error changeAttachmentContentById(const AttachmentEntityBase::EntityId         attachmentId,
                                      const std::unique_ptr<AttachmentContentBase> &attachmentContent,
                                      std::unique_ptr<AttachmentLocalFileLink>     &changedAttachment);
    
    Error getAttachmentBytesByLocalPath(const QUrl &filePath,
                                        QByteArray &fileBytes);
    Error getAttachmentContentByLocalPath(const QUrl &filePath,
                                          std::unique_ptr<AttachmentContentBase> &attachmentContent);
    
    Error storeAttachmentContentInFileById(const EntityInterface::EntityId              attachmentId,
                                           const std::unique_ptr<AttachmentContentBase> &attachmentContentToStore,
                                           std::unique_ptr<AttachmentLocalFileLink>     &storedAttachment);
    Error storeAttachmentLocalFileUrlById(const EntityInterface::EntityId attachmentId,
                                          const QUrl &attachmentLocalUrl,
                                          std::unique_ptr<AttachmentLocalFileLink> &storedAttachment);
    Error loadAttachmentContentById(const AttachmentEntityBase::EntityId   attachmentId,
                                    std::unique_ptr<AttachmentContentBase> &storedAttachment);
    
    Error getAttachmentLocalFileLinkById(const AttachmentEntityBase::EntityId     attachmentId,
                                         std::unique_ptr<AttachmentLocalFileLink> &storedAttachment);
    
protected:
    std::unique_ptr<NetworkAttachmentFacadeInterface> m_attachmentsFacade;
//    std::unique_ptr<AttachmentDefinerBase>            m_attachmentDefiner;
    
    AttachmentsLocalFilesHash m_attachmentsHash;
};

#endif // ATTACHMENTMANAGERBASE_H
