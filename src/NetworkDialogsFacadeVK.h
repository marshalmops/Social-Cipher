#ifndef NETWORKDIALOGSFACADEVK_H
#define NETWORKDIALOGSFACADEVK_H

#include "NetworkDialogsFacadeInterface.h"
#include "NetworkFacadeVK.h"
#include "EntityJsonParserVK.h"

class NetworkDialogsFacadeVK
        : public NetworkDialogsFacadeInterface,
          public NetworkFacadeVK
{
public:
    NetworkDialogsFacadeVK(const std::shared_ptr<EntityJsonParserVK> &parser, 
                           const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    
    virtual Error getDialogs (std::vector<DialogEntity> &dialogs) override;
    virtual Error getPeerName(const DialogEntity::EntityId peerId, 
                              QString &peerName) override; 
    
    virtual bool initialize() override;
};

#endif // NETWORKDIALOGSFACADEVK_H
