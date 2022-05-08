#include "NetworkMessagesEventCycledChecker.h"

NetworkMessagesEventCycledChecker::NetworkMessagesEventCycledChecker(std::unique_ptr<NetworkDialogMessagesFacadeInterface> &&dialogMessagesFacade)
    : NetworkCycledCheckerInterface{std::move(dialogMessagesFacade)}
{
    
}

Error NetworkMessagesEventCycledChecker::check(std::vector<std::shared_ptr<MessageEntityBase>> &newMessages)
{
    std::vector<std::shared_ptr<MessageEntityBase>> newMessagesBuffer;
    
    auto err = m_dialogMessagesFacade->tryGetMessages(newMessagesBuffer);
    
    if (err.isValid())             return err;
    if (newMessagesBuffer.empty()) return Error{};
    
    newMessages = std::move(newMessagesBuffer);
    
    return Error{};
}
