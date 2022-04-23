#include "NetworkCycledCheckerInterface.h"

NetworkCycledCheckerInterface::NetworkCycledCheckerInterface(std::unique_ptr<NetworkDialogMessagesFacadeInterface> &&dialogMessagesFacade)
    : m_dialogMessagesFacade{dialogMessagesFacade.release()}
{
    
}
