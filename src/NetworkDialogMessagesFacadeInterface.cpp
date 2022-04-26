#include "NetworkDialogMessagesFacadeInterface.h"

const QHash<NetworkDialogMessagesFacadeInterface::CommandCode, QString> &NetworkDialogMessagesFacadeInterface::getCommandsHash()
{
    static QHash<CommandCode, QString> commandsHash = {
        {CommandCode::CC_START_ENCRYPTION_INIT, "SENI"},
        {CommandCode::CC_END_ENCRYPTION_INIT,   "EENI"},
        {CommandCode::CC_RESET_ENCRYPTION,      "REEN"}
    };
    
    return commandsHash;
}

const NetworkDialogMessagesFacadeInterface::CommandCode NetworkDialogMessagesFacadeInterface::getCommandCodeByString(const QString &command)
{
    const auto &commandsHash = getCommandsHash();
    
    foreach (const auto &curCommand, commandsHash.keys()) {
        if (command == commandsHash.value(curCommand))
            return curCommand;
    }
    
    return CommandCode::CC_INVALID;
}

const QString NetworkDialogMessagesFacadeInterface::getStringByCommandCode(const CommandCode code)
{
    const auto &commandsHash = getCommandsHash();
    
    if (!commandsHash.contains(code)) return QString();
    
    return commandsHash.value(code);
}

bool NetworkDialogMessagesFacadeInterface::getCommandByMessage(const MessageEntity &message,
                                                               CommandCode &commandCode,
                                                               QString &commandContent) const
{
    auto fullCommandLength = C_COMMAND_LENGTH + strlen(C_COMMAND_START);
    QString messageContent = message.getText();
    
    if (messageContent.length() < fullCommandLength)
        return true;
    
    QString strBegin{messageContent.mid(0, fullCommandLength)};
    
    if (strBegin.mid(0, strlen(C_COMMAND_START)) != QString(C_COMMAND_START))
        return true;
    
    QString commandStr = strBegin.mid(strlen(C_COMMAND_START));
    
    commandCode = getCommandCodeByString(commandStr);
    
    if (commandCode == CommandCode::CC_INVALID)
        return false;
    
    commandContent = messageContent.mid(fullCommandLength);
    
    return true;
}

NetworkDialogMessagesFacadeInterface::NetworkDialogMessagesFacadeInterface(const std::shared_ptr<EntityJsonParserInterface> &entityParser,
                                                                           const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : NetworkFacadeInterface{executor},
      NetworkFacadeUsingEntityParser{entityParser}
{
    
}

Error NetworkDialogMessagesFacadeInterface::sendCommand(const CommandCode commandCode,
                                                        const QString commandContent,
                                                        const EntityInterface::EntityId peerId) const
{
    MessageEntity commandMessage = generateCommandMessage(commandCode,
                                                          commandContent);
    
    return sendMessage(commandMessage, peerId);
}

MessageEntity NetworkDialogMessagesFacadeInterface::generateCommandMessage(const CommandCode command,
                                                                           const QString &content) const
{
    QString messageText = QString(C_COMMAND_START) + getStringByCommandCode(command) + content;
    MessageEntity message{messageText};
    
    return message;
}
