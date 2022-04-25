#ifndef NETWORKFACADEINTERFACE_H
#define NETWORKFACADEINTERFACE_H

#include <memory>

#include "Error.h"
#include "NetworkRequestExecutorInterface.h"

class NetworkFacadeInterface
{
public:
    NetworkFacadeInterface(const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    virtual ~NetworkFacadeInterface() = default;
    
    virtual bool initialize() = 0;
    
    std::shared_ptr<NetworkRequestExecutorInterface> getExecutor() const;
    Error getLastError() const;
    
    void abortCurrentRequest();
    
protected:
    std::shared_ptr<NetworkRequestExecutorInterface> m_requestExecutor;
    mutable Error m_lastError;
};

#endif // NETWORKFACADEINTERFACE_H
