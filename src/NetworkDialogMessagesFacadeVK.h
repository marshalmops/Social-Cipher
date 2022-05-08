#ifndef NETWORKDIALOGMESSAGESFACADEVK_H
#define NETWORKDIALOGMESSAGESFACADEVK_H

#include "NetworkDialogMessagesFacadeInterface.h"
#include "NetworkFacadeVK.h"

#include "MessageJsonParserVK.h"

#include "RelatedToMessagesProcessingVK.h"

class NetworkDialogMessagesFacadeVK
        : public NetworkDialogMessagesFacadeInterface,
          public NetworkFacadeVK
{
public:
    NetworkDialogMessagesFacadeVK(std::unique_ptr<IncomingMessagesProcessorBase> &messagesProcessor,
                                  const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    
    virtual Error sendMessage   (const std::shared_ptr<MessageEntityBase> &message,
                                 const EntityInterface::EntityId peerId) const override;
    virtual Error tryGetMessages(std::vector<std::shared_ptr<MessageEntityBase>> &messages) override;
    virtual Error getDialogMessages(const EntityInterface::EntityId peerId, 
                                    std::vector<std::shared_ptr<MessageEntityBase>> &messages,
                                    const uint8_t messagesCount = C_DEFAULT_MESSAGES_COUNT) override;    
    
    virtual bool initialize() override;
    
protected:
    virtual std::shared_ptr<MessageEntityBase> generateCommandMessage(const CommandCode command,
                                                                      const QString &content) const override;
    
    Error getMessageById(const EntityInterface::EntityId peerId,
                         const MessageEntityBase::EntityId messageId,
                         std::shared_ptr<MessageEntityBase> &gottenMessage);
    
private:
    uint64_t m_lastTs;
};

#endif // NETWORKDIALOGMESSAGESFACADEVK_H
