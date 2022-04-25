#include "NetworkCycledCheckerInterface.h"

NetworkCycledCheckerInterface::NetworkCycledCheckerInterface(std::unique_ptr<NetworkDialogMessagesFacadeInterface> &&dialogMessagesFacade)
    : m_dialogMessagesFacade{dialogMessagesFacade.release()}
{
    
}

void NetworkCycledCheckerInterface::abortCurrentCheck()
{
    m_dialogMessagesFacade->abortCurrentRequest();
}
