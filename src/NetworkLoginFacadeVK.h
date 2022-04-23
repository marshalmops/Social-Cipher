#ifndef NETWORKLOGINFACADEVK_H
#define NETWORKLOGINFACADEVK_H

#include "NetworkLoginFacadeInterface.h"
#include "NetworkFacadeVK.h"

class NetworkLoginFacadeVK
        : public NetworkLoginFacadeInterface,
          public NetworkFacadeVK
{
public:
    NetworkLoginFacadeVK(const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    
    //virtual Error getToken  (NetworkSettings::AuthToken &token) override;
    virtual Error initializeWithToken(const QString token) override;

    virtual bool initialize() override;
};

#endif // NETWORKLOGINFACADEVK_H
