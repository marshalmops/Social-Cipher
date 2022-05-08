#include "NetworkMessagesTraditionCycledChecker.h"

NetworkMessagesTraditionCycledChecker::NetworkMessagesTraditionCycledChecker(std::unique_ptr<NetworkDialogsFacadeInterface> &&dialogsFacade,
                                                                             std::unique_ptr<NetworkDialogMessagesFacadeInterface> &&dialogMessagesFacade)
    : NetworkCycledCheckerInterface{std::move(dialogMessagesFacade)},
      m_dialogsFacade{dialogsFacade.release()}
{
    
}

Error NetworkMessagesTraditionCycledChecker::check(std::vector<std::shared_ptr<MessageEntityBase>> &newMessages)
{
    // getting dialogs list...
    
    std::vector<std::unique_ptr<DialogEntityBase>> dialogsBuffer;
    
    auto err = m_dialogsFacade->getDialogs(dialogsBuffer);
    
    if (err.isValid()) return err;
    
    if (m_lastMessagesOfDialogs.empty())
        return initializeLastMessagesOfDialogs(dialogsBuffer);
    
//    // getting messages list for every dialog...
    
//    auto newMessagesAddingLambda = 
//            [](std::vector<std::unique_ptr<MessageEntityBase>> &dialogMessages,
//               std::vector<std::shared_ptr<MessageEntityBase>> &newMessages)
//    {
//        for (auto messageIter = dialogMessages.begin(); messageIter != dialogMessages.end(); ++messageIter) {
//            newMessages.push_back(std::shared_ptr<MessageEntityBase>(messageIter->release()));
//        }
//    };
    
    std::vector<std::shared_ptr<MessageEntityBase>> newMessagesBuffer;
    
    for (auto dialogIter = dialogsBuffer.begin(); dialogIter != dialogsBuffer.end(); ++dialogIter) {
        auto &dialog{*dialogIter};
        
        if (!dialog->isValid()) return Error{"Gotten dialog is invalid!", true};
        
        // checking for last messages by the last message id:
        
        std::vector<std::shared_ptr<MessageEntityBase>> curDialogMessages;
        
        err = m_dialogMessagesFacade->getDialogMessages(dialog->getPeerId(), curDialogMessages);
        
        if (err.isValid()) return Error{"Dialog messages getting error!", true};
        
        if (!m_lastMessagesOfDialogs.contains(dialog->getPeerId())) {
            newMessages = std::move(curDialogMessages);
            //newMessagesAddingLambda(curDialogMesages, newMessages);
            
            return Error{};
        }
        
        for (auto messageIter = curDialogMessages.begin(); messageIter != curDialogMessages.end(); ++messageIter) {
            auto &message{*messageIter};
            
            if (!message->isValid()) return Error{"Gotten message is invalid!", true};
            
            if (message->getMessageId() > m_lastMessagesOfDialogs[dialog->getPeerId()])
                newMessagesBuffer.push_back(std::move(message));
        }
    }
    
    newMessages = std::move(newMessagesBuffer);
    
    //newMessagesAddingLambda(newMessagesBuffer, newMessages);
    
    return Error{};
}

Error NetworkMessagesTraditionCycledChecker::initializeLastMessagesOfDialogs(const std::vector<std::unique_ptr<DialogEntityBase>> &dialogs)
{
    for (auto dialogIter = dialogs.begin(); dialogIter != dialogs.end(); ++dialogIter) {
        auto &dialog{*dialogIter};
        
        if (!dialog->isValid()) return Error{"Gotten dialog is invalid!", true};
        
        std::vector<std::shared_ptr<MessageEntityBase>> curDialogMessages;
        
        auto err = m_dialogMessagesFacade->getDialogMessages(dialog->getPeerId(), curDialogMessages);
        
        if (err.isValid())             return err;
        if (curDialogMessages.empty()) return Error{};
        
        auto lastMessageOfDialog = *(curDialogMessages.back());
        
        m_lastMessagesOfDialogs[dialog->getPeerId()] = lastMessageOfDialog.getMessageId();
    }
    
    return Error{};
}
