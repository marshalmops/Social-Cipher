#ifndef NETWORKDIALOGMESSAGESFACADEVK_H
#define NETWORKDIALOGMESSAGESFACADEVK_H

#include "NetworkDialogMessagesFacadeInterface.h"
#include "NetworkFacadeVK.h"

#include "EntityJsonParserVK.h"

class NetworkDialogMessagesFacadeVK
        : public NetworkDialogMessagesFacadeInterface,
          public NetworkFacadeVK
{
public:
    NetworkDialogMessagesFacadeVK(const std::shared_ptr<EntityJsonParserVK> &parser, 
                                  const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    
    virtual Error sendMessage   (const MessageEntity &message,
                                 const EntityInterface::EntityId peerId) const override;
    virtual Error tryGetMessages(std::vector<MessageEntity> &messages) override;
    virtual Error getDialogMessages(const EntityInterface::EntityId peerId, std::vector<MessageEntity> &messages) override;    
    
    virtual bool initialize() override;
    
private:
    uint64_t m_lastTs;
    
};

#endif // NETWORKDIALOGMESSAGESFACADEVK_H
