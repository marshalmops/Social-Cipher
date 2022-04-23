#include "NetworkFacadeInterface.h"

NetworkFacadeInterface::NetworkFacadeInterface(const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : m_requestExecutor{executor}
{
    
}

std::shared_ptr<NetworkRequestExecutorInterface> NetworkFacadeInterface::getExecutor() const
{
    return m_requestExecutor;
}
