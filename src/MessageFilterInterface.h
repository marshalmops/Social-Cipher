#ifndef MESSAGEFILTERINTERFACE_H
#define MESSAGEFILTERINTERFACE_H

#include "MessageEntityBase.h"
#include "RelatedToMessagesProcessingBase.h"

#include "AttachmentDefinerBase.h"

#include "NetworkSettings.h"

class MessageFilterInterface
{
public:
    MessageFilterInterface() = default;
    
    virtual bool filterMessages(const std::vector<std::shared_ptr<MessageEntityBase>> &messagesToFilter,
                                const RelatedToMessagesProcessingBase::EntityProcessingFlagType messagesProcessingFlag,
                                std::vector<std::shared_ptr<MessageEntityBase>> &filteredMessages) = 0;
protected:
    virtual bool isAttachmentExtensionAllowed(const QString &attachmentExtension) const = 0;
};

#endif // MESSAGEFILTERINTERFACE_H
