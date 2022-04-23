#include "NetworkMessagesTraditionCycledChecker.h"

NetworkMessagesTraditionCycledChecker::NetworkMessagesTraditionCycledChecker(std::unique_ptr<NetworkDialogsFacadeInterface> &&dialogsFacade,
                                                                             std::unique_ptr<NetworkDialogMessagesFacadeInterface> &&dialogMessagesFacade)
    : NetworkCycledCheckerInterface{std::move(dialogMessagesFacade)},
      m_dialogsFacade{dialogsFacade.release()}
{
    
}

Error NetworkMessagesTraditionCycledChecker::check(std::vector<MessageEntity> &newMessages)
{
    // getting dialogs list...
    
    std::vector<DialogEntity> dialogsBuffer;
    
    auto err = m_dialogsFacade->getDialogs(dialogsBuffer);
    
    if (err.isValid()) return err;
    
    if (m_lastMessagesOfDialogs.empty())
        return initializeLastMessagesOfDialogs(dialogsBuffer);
    
    // getting messages list for every dialog...
    
    std::vector<MessageEntity> newMessagesBuffer;
    
    foreach (const auto &dialog, dialogsBuffer) {
        if (!dialog.isValid()) return Error{"Gotten dialog is invalid!", true};
        
        // checking for last messages by the last message id:
        
        std::vector<MessageEntity> curDialogMesages;
        
        err = m_dialogMessagesFacade->getDialogMessages(dialog.getPeerId(), curDialogMesages);
        
        if (err.isValid()) return Error{"Dialog messages getting error!", true};
        
        if (!m_lastMessagesOfDialogs.contains(dialog.getPeerId())) {
            newMessages = std::move(curDialogMesages);
            
            return Error{};
        }
        
        foreach (const auto &message, curDialogMesages) {
            if (!message.isValid()) return Error{"Gotten message is invalid!", true};
            
            if (message.getMessageId() > m_lastMessagesOfDialogs[dialog.getPeerId()])
                newMessagesBuffer.push_back(message);
        }
    }
    
    newMessages = std::move(newMessagesBuffer);
    
    return Error{};
}

Error NetworkMessagesTraditionCycledChecker::initializeLastMessagesOfDialogs(const std::vector<DialogEntity> &dialogs)
{
    foreach (const auto &dialog, dialogs) {
        if (!dialog.isValid()) return Error{"Gotten dialog is invalid!", true};
        
        std::vector<MessageEntity> curDialogMessages;
        
        auto err = m_dialogMessagesFacade->getDialogMessages(dialog.getPeerId(), curDialogMessages);
        
        if (err.isValid())             return err;
        if (curDialogMessages.empty()) return Error{};
        
        auto lastMessageOfDialog = curDialogMessages.back();
        
        m_lastMessagesOfDialogs[dialog.getPeerId()] = lastMessageOfDialog.getMessageId();
    }
    
    return Error{};
}
