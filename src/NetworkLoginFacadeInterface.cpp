#include "NetworkLoginFacadeInterface.h"

NetworkLoginFacadeInterface::NetworkLoginFacadeInterface(const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : NetworkFacadeInterface{executor}
{
    
}
