#include "NetworkFacadeInterface.h"

NetworkFacadeInterface::NetworkFacadeInterface(const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : m_requestExecutor{executor},
      m_lastError      {}
{
    
}

std::shared_ptr<NetworkRequestExecutorInterface> NetworkFacadeInterface::getExecutor() const
{
    return m_requestExecutor;
}

Error NetworkFacadeInterface::getLastError() const
{
    return m_lastError;
}

void NetworkFacadeInterface::abortCurrentRequest()
{
    m_requestExecutor->abortCurrentRequest();
}
