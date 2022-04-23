#ifndef NETWORKDIALOGMESSAGESFACADEINTERFACE_H
#define NETWORKDIALOGMESSAGESFACADEINTERFACE_H

#include <QUrlQuery>

#include "NetworkFacadeInterface.h"
#include "NetworkFacadeUsingEntityParser.h"
#include "MessageEntity.h"

class NetworkDialogMessagesFacadeInterface
        : public NetworkFacadeInterface,
          public NetworkFacadeUsingEntityParser
{
public:
    NetworkDialogMessagesFacadeInterface(const std::shared_ptr<EntityJsonParserInterface> &entityParser,
                                         const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    
    virtual Error sendMessage      (const MessageEntity &message, 
                                    const EntityInterface::EntityId peerId) const = 0;
    virtual Error tryGetMessages   (std::vector<MessageEntity> &messages) = 0;
    virtual Error getDialogMessages(const EntityInterface::EntityId peerId,
                                    std::vector<MessageEntity> &messages) = 0;
};

#endif // NETWORKDIALOGMESSAGESFACADEINTERFACE_H
