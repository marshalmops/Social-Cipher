#ifndef NETWORKDIALOGSFACADEVK_H
#define NETWORKDIALOGSFACADEVK_H

#include "NetworkDialogsFacadeInterface.h"
#include "NetworkFacadeVK.h"

#include "DialogJsonParserVK.h"

class NetworkDialogsFacadeVK
        : public NetworkDialogsFacadeInterface,
          public NetworkFacadeVK
{
public:
    NetworkDialogsFacadeVK(const std::shared_ptr<DialogJsonParserVK> &dialogsParser, 
                           const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    
    virtual Error getDialogs (std::vector<std::unique_ptr<DialogEntityBase>> &dialogs) override;
    virtual Error getPeerName(const DialogEntityBase::EntityId peerId, 
                              QString &peerName) override; 
    
    virtual bool initialize() override;
};

#endif // NETWORKDIALOGSFACADEVK_H
