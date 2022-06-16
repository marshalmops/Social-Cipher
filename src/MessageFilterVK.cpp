#include "MessageFilterVK.h"

MessageFilterVK::MessageFilterVK()
{
    
}

bool MessageFilterVK::filterMessages(const std::vector<std::shared_ptr<MessageEntityBase> > &messagesToFilter,
                                     const RelatedToMessagesProcessingBase::EntityProcessingFlagType messagesProcessingFlag,
                                     std::vector<std::shared_ptr<MessageEntityBase>> &filteredMessages)
{
    bool result{true};
    
    switch (messagesProcessingFlag) {
        case RelatedToMessagesProcessingVK::MessageProcessingFlag::MPF_IS_EVENT: {
            result = filterMessagesFromEventsByAttachments(messagesToFilter, filteredMessages);
            
            break;
        }
        default: {
            result = filterMessagesFromStandardByAttachments(messagesToFilter, filteredMessages);
        }
    }
    
    return result;
}

bool MessageFilterVK::filterMessagesFromEventsByAttachments(const std::vector<std::shared_ptr<MessageEntityBase>> &messagesToFilter,
                                                            std::vector<std::shared_ptr<MessageEntityBase>> &filteredMessages)
{
    for (auto messageIter = messagesToFilter.begin(); messageIter != messagesToFilter.end(); ++messageIter) {
        auto &message{*messageIter};
        MessageEntityBase::AttachmentsList filteredAttachmentsToGet;
        
        foreach (const auto &attachment, message->getAttachments()) {
            if (!isAttachmentTypeAllowed(dynamic_cast<AttachmentEntityVKUsingIdString*>(attachment.get())->getType()))
                continue;
            
            filteredAttachmentsToGet.push_back(attachment);
        }
        
        MessageEntityBase filteredMessage{message->getText(), filteredAttachmentsToGet, message->isEncrypted(), message->getFromId(), message->getPeerId(),
                                          message->getMessageId(), message->getTime()};
        
        if (!filteredMessage.isValid()) continue;
        
        filteredMessages.push_back(std::make_shared<MessageEntityBase>(std::move(filteredMessage)));
    }
    
    return true;
}

bool MessageFilterVK::filterMessagesFromStandardByAttachments(const std::vector<std::shared_ptr<MessageEntityBase>> &messagesToFilter,
                                                              std::vector<std::shared_ptr<MessageEntityBase>> &filteredMessages)
{
    for (auto messageIter = messagesToFilter.begin(); messageIter != messagesToFilter.end(); ++messageIter) {
        auto &message{*messageIter};
        MessageEntityBase::AttachmentsList filteredAttachmentsToDownload;
        
        foreach (const auto &attachment, message->getAttachments()) {
            AttachmentEntityVKUsingLink* attachmentVKUsingLink{dynamic_cast<AttachmentEntityVKUsingLink*>(attachment.get())};
            QString                      attachmentExtension  {attachmentVKUsingLink->getExtension()};
            
            if (!isAttachmentExtensionAllowed(attachmentExtension))
                continue;
            
            filteredAttachmentsToDownload.push_back(attachment);
        }
        
        MessageEntityBase filteredMessage{message->getText(), filteredAttachmentsToDownload, message->isEncrypted(), message->getFromId(), message->getPeerId(),
                                          message->getMessageId(), message->getTime()};
        
        if (!filteredMessage.isValid()) continue;
        
        filteredMessages.push_back(std::make_shared<MessageEntityBase>(std::move(filteredMessage)));
    }
    
    return true;
}

bool MessageFilterVK::isAttachmentExtensionAllowed(const QString &attachmentExtension) const
{
    if (attachmentExtension.isEmpty()) return false;
    
    static const QVector<QString> allowedExtensionsHash = {
        "jpg", "png", "txt", "docx", "doc", NetworkSettings::C_ENCODED_ATTACHMENT_EXTENSION
    };
    
    return (allowedExtensionsHash.contains(attachmentExtension) ? true : false);
}

bool MessageFilterVK::isAttachmentTypeAllowed(const QString &attachmentType) const
{
    if (attachmentType.isEmpty()) return false;
    
    static const QVector<QString> allowedTypesHash = {
        "photo", "doc"
    };
    
    return (allowedTypesHash.contains(attachmentType) ? true : false);
}
