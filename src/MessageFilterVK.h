#ifndef MESSAGEFILTERVK_H
#define MESSAGEFILTERVK_H

#include "MessageFilterInterface.h"
#include "MessageEntityBase.h"

#include "AttachmentEntityVKUsingIdString.h"
#include "AttachmentEntityVKUsingLink.h"

#include "RelatedToMessagesProcessingVK.h"

class MessageFilterVK : public MessageFilterInterface
{
public:
    MessageFilterVK();
    
    virtual bool filterMessages(const std::vector<std::shared_ptr<MessageEntityBase>> &messagesToFilter,
                                const RelatedToMessagesProcessingBase::EntityProcessingFlagType messagesProcessingFlag,
                                std::vector<std::shared_ptr<MessageEntityBase>> &filteredMessages) override;
    virtual bool isAttachmentExtensionAllowed(const QString &attachmentExtension) const override;
    
protected:
    bool filterMessagesFromEventsByAttachments(const std::vector<std::shared_ptr<MessageEntityBase>> &messagesToFilter,
                                               std::vector<std::shared_ptr<MessageEntityBase>> &filteredMessages);
    bool filterMessagesFromStandardByAttachments(const std::vector<std::shared_ptr<MessageEntityBase>> &messagesToFilter,
                                                 std::vector<std::shared_ptr<MessageEntityBase>> &filteredMessages);

    bool isAttachmentTypeAllowed(const QString &attachmentType) const;
    
};

#endif // MESSAGEFILTERVK_H
