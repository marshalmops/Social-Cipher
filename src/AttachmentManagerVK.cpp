#include "AttachmentManagerVK.h"

AttachmentManagerVK::AttachmentManagerVK(std::unique_ptr<NetworkAttachmentFacadeInterface> &attachmentsFacade)
    : AttachmentManagerBase{attachmentsFacade}
{
    
}

//Error AttachmentManagerVK::processNewAttachments(const std::vector<std::unique_ptr<AttachmentEntityBase>> &attachmentsToProcess,
//                                                 std::vector<std::unique_ptr<AttachmentLocalFileLink> > &attachmentsLocalFileLinks)
//{
//    Error err{"New attachment processing error!", true};
    
//    foreach (const auto &attachment, attachmentsToProcess) {
//        std::unique_ptr curAttachmentVK = std::unique_ptr<AttachmentEntityVK>(dynamic_cast<AttachmentEntityVK*>(attachment.get()));
        
//        switch (curAttachmentVK->getProcessingFlag()) {
//            case RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_EVENTIONAL: {
//                if ((err = processAttachmentFromEvent(curAttachmentVK)).isValid()) return err;
                
//                break;
//            }
//            default: {
//                if ((err = processAttachmentFromStandard(curAttachmentVK)).isValid()) return err;
//            }
//        }
//    }
    
//    return Error{};
//}

Error AttachmentManagerVK::uploadNewAttachment(const std::unique_ptr<AttachmentContentBase> &attachmentContentToUpload,
                                               std::shared_ptr<AttachmentEntityBase> &uploadedAttachment)
{
    Error err{"Attachment uploading error!", true};
    
    switch (AttachmentDefinerVK::getContentExtensionIdByExtension(AttachmentDefinerVK::getExtensionByFilename(attachmentContentToUpload->getFileName()))) {
        case AttachmentDefinerVK::ContentExtensionId::CEI_JPG:
        case AttachmentDefinerVK::ContentExtensionId::CEI_PNG:
        case AttachmentDefinerVK::ContentExtensionId::CEI_GIF: {
            if ((err = m_attachmentsFacade->uploadImage(attachmentContentToUpload, 0, uploadedAttachment)).isValid())
                return err;
            
            break;
        }
        case AttachmentDefinerVK::ContentExtensionId::CEI_ENCRYPTED:
        case AttachmentDefinerVK::ContentExtensionId::CEI_DOC:
        case AttachmentDefinerVK::ContentExtensionId::CEI_DOCX:
        case AttachmentDefinerVK::ContentExtensionId::CEI_TXT:{
            if ((err = m_attachmentsFacade->uploadDocument(attachmentContentToUpload, 0, uploadedAttachment)).isValid())
                return err;
            
            break;
        }
        default: return Error{"Attachment extension is invalid!"};
    }
    
//    // saving bytes to attachments hash:
    
//    AttachmentEntityVKUsingIdString *uploadedAttachmentPtr{dynamic_cast<AttachmentEntityVKUsingIdString*>(uploadedAttachment.get())};
    
//    if (m_attachmentsHash.contains(uploadedAttachmentPtr->getAttachmentId()))
//        return err;
    
//    m_attachmentsHash[uploadedAttachmentPtr->getAttachmentId()] = std::move(attachmentContentToUpload);
    
    return Error{};
}

Error AttachmentManagerVK::downloadNewAttachment(const std::shared_ptr<AttachmentEntityBase> &attachmentToDownload,
                                                 std::unique_ptr<AttachmentContentBase> &downloadedAttachment)
{
    Error err{"Attachment downloading error!"};
    
    AttachmentEntityVK *attachmentVK{dynamic_cast<AttachmentEntityVK*>(attachmentToDownload.get())};
    
    // getting type of file:
    
    RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag attachmentProcessingWay{RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_INVALID};
    
    switch (attachmentVK->getProcessingFlag()) {
        case RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_EVENTIONAL: {
            AttachmentEntityVKUsingIdString *attachmentVKUsingIdString{dynamic_cast<AttachmentEntityVKUsingIdString*>(attachmentVK)};
            
            attachmentProcessingWay = AttachmentDefinerVK::getProcessingWayByTypeString(attachmentVKUsingIdString->getType());
            
            break;
        }
        case RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_STANDARD: {
            AttachmentEntityVKUsingLink *attachmentVKUsingLink{dynamic_cast<AttachmentEntityVKUsingLink*>(attachmentVK)};
        
            attachmentProcessingWay = AttachmentDefinerVK::getProcessingWayByExtension(attachmentVKUsingLink->getExtension());
            
            break;
        }    
        default: return err;
    }
    
    if (attachmentProcessingWay == RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_INVALID)
        return err;
    
    std::unique_ptr<AttachmentContentBase> downloadedAttachmentBuffer{};
    
    if ((err = m_attachmentsFacade->downloadAttachment(attachmentToDownload, attachmentProcessingWay, downloadedAttachmentBuffer)).isValid())
        return err;
    
    if (!downloadedAttachmentBuffer->isValid()) return err;
    
    downloadedAttachment = std::move(downloadedAttachmentBuffer);
    
    return Error{};
}

//Error AttachmentManagerVK::processAttachmentFromEvent(std::unique_ptr<AttachmentEntityVK> &attachment)
//{
//    Error err{"Attachment processing error!", true};
    
//    AttachmentEntityVKUsingIdString *attachmentFromEvent{dynamic_cast<AttachmentEntityVKUsingIdString*>(attachment.get())};
//    NetworkAttachmentFacadeVK   *attachmentFacadeVK {getAttachmentFacadeVK()};
    
//    QString attachmentIdString{attachmentFromEvent->getIdString()};
//    QUrl    attachmentUrl{};
    
//    RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag attachmentProcessingWayFlag{AttachmentDefinerVK::getProcessingWayByTypeString(attachmentFromEvent->getType())};
    
//    if (attachmentProcessingWayFlag == RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_INVALID)
//        return err;
    
//    if ((err = attachmentFacadeVK->getAttachmentUrlById(attachmentIdString, attachmentProcessingWayFlag, attachmentUrl)).isValid())
//        return err;
            
//    attachment = std::make_unique<AttachmentEntityVKUsingLink>(attachment->getAttachmentId(),
//                                                                  attachment->getPeerId(),
//                                                                  attachmentUrl);
    
//    return processAttachmentFromStandard(attachment);
//}

//Error AttachmentManagerVK::processAttachmentFromStandard(std::unique_ptr<AttachmentEntityVK> &attachment)
//{
//    Error err{"Attachment processing error!", true};
    
//    AttachmentEntityVKUsingLink *attachmentFromStandard{dynamic_cast<AttachmentEntityVKUsingLink*>(attachment.get())};
    
//    QMetaType::Type metaType{AttachmentDefinerBase::getMetaTypeByFilename(attachmentFromStandard->getUrl().fileName())};
//    QByteArray      attachmentBytes{};
    
//    if ((err = m_attachmentsFacade->downloadAttachment(attachmentFromStandard->getUrl(), attachmentBytes)).isValid())
//        return err;
    
//    if (attachmentBytes.isEmpty()) return err;
    
//    AttachmentContentBase attachmentContent{metaType, attachmentFromStandard->getExtension(), attachmentBytes};
    
//    m_attachmentsHash[attachment->getAttachmentId()] = std::move(attachmentContent);
    
//    return Error{};
//}

NetworkAttachmentFacadeVK* AttachmentManagerVK::getAttachmentFacadeVK() const
{
    return dynamic_cast<NetworkAttachmentFacadeVK*>(m_attachmentsFacade.get());
}
