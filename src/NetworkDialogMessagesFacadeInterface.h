#ifndef NETWORKDIALOGMESSAGESFACADEINTERFACE_H
#define NETWORKDIALOGMESSAGESFACADEINTERFACE_H

#include <QUrlQuery>

#include "NetworkFacadeInterface.h"
//#include "NetworkFacadeUsingEntityParser.h"
#include "MessageJsonParserInterface.h"

#include "MessageFilterInterface.h"

#include "MessageEntityBase.h"

#include "IncomingMessagesProcessorBase.h"

class NetworkDialogMessagesFacadeInterface
        : public NetworkFacadeInterface/*,
          public NetworkFacadeUsingEntityParser*/
{
public:
    constexpr static const char*   C_COMMAND_START  = "__";
    constexpr static const uint8_t C_COMMAND_LENGTH = 4;
    
    constexpr static const uint8_t C_DEFAULT_MESSAGES_COUNT = 20;
    
    enum CommandCode : uint8_t {
        CC_INVALID = 0,
        CC_START_ENCRYPTION_INIT,
        CC_END_ENCRYPTION_INIT,
        CC_START_CONTENT_ENCRYPTION,
        CC_RESET_ENCRYPTION
    };
    
    static const QHash<CommandCode, QString>& getCommandsHash();
    
    static const CommandCode getCommandCodeByString(const QString &command);
    static const QString     getStringByCommandCode(const CommandCode code);
    
    bool getCommandByMessage(const std::shared_ptr<MessageEntityBase> &message,
                             CommandCode &commandCode,
                             QString &commandContent) const;
    
    NetworkDialogMessagesFacadeInterface(std::unique_ptr<IncomingMessagesProcessorBase> &messagesProcessor,
                                         const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    
    virtual Error sendMessage      (const std::shared_ptr<MessageEntityBase> &message, 
                                    const EntityInterface::EntityId peerId) const = 0;
    virtual Error tryGetMessages   (std::vector<std::shared_ptr<MessageEntityBase>> &messages) = 0;
    virtual Error getDialogMessages(const EntityInterface::EntityId peerId,
                                    std::vector<std::shared_ptr<MessageEntityBase>> &messages,
                                    const uint8_t messagesCount = C_DEFAULT_MESSAGES_COUNT) = 0;

    Error sendCommand(const CommandCode commandCode,
                      const QString     commandContent,
                      const EntityInterface::EntityId peerId) const;
    
protected:
    virtual std::shared_ptr<MessageEntityBase> generateCommandMessage(const CommandCode command,
                                                                      const QString &content) const = 0;
    
    std::unique_ptr<IncomingMessagesProcessorBase> m_messagesProcessor;
};

#endif // NETWORKDIALOGMESSAGESFACADEINTERFACE_H
