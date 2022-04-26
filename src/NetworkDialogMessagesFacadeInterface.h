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
    constexpr static const char*   C_COMMAND_START  = "__";
    constexpr static const uint8_t C_COMMAND_LENGTH = 4;
    
    enum CommandCode : uint8_t {
        CC_INVALID = 0,
        CC_START_ENCRYPTION_INIT,
        CC_END_ENCRYPTION_INIT,
        CC_RESET_ENCRYPTION
    };
    
    static const QHash<CommandCode, QString>& getCommandsHash();
    
    static const CommandCode getCommandCodeByString(const QString &command);
    static const QString     getStringByCommandCode(const CommandCode code);
    
    bool getCommandByMessage(const MessageEntity &message,
                             CommandCode &commandCode,
                             QString &commandContent) const;
    
    NetworkDialogMessagesFacadeInterface(const std::shared_ptr<EntityJsonParserInterface> &entityParser,
                                         const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    
    virtual Error sendMessage      (const MessageEntity &message, 
                                    const EntityInterface::EntityId peerId) const = 0;
    virtual Error tryGetMessages   (std::vector<MessageEntity> &messages) = 0;
    virtual Error getDialogMessages(const EntityInterface::EntityId peerId,
                                    std::vector<MessageEntity> &messages) = 0;

    Error sendCommand(const CommandCode commandCode,
                      const QString     commandContent,
                      const EntityInterface::EntityId peerId) const;
    
protected:
    MessageEntity generateCommandMessage(const CommandCode command,
                                         const QString     &content) const;
};

#endif // NETWORKDIALOGMESSAGESFACADEINTERFACE_H
