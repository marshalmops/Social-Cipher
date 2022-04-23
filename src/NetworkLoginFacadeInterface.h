#ifndef NETWORKLOGINFACADEINTERFACE_H
#define NETWORKLOGINFACADEINTERFACE_H

#include "NetworkFacadeInterface.h"
#include "NetworkSettings.h"

class NetworkLoginFacadeInterface : public NetworkFacadeInterface
{
public:
    NetworkLoginFacadeInterface(const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    
    //virtual Error getToken  (NetworkSettings::AuthToken &token) = 0;
    virtual Error initializeWithToken(const QString token) = 0;
    
};

#endif // NETWORKLOGINFACADEINTERFACE_H
