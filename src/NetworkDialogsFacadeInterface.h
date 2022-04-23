#ifndef NETWORKDIALOGSFACADEINTERFACE_H
#define NETWORKDIALOGSFACADEINTERFACE_H

#include "NetworkFacadeInterface.h"
#include "NetworkFacadeUsingEntityParser.h"
#include "DialogEntity.h"

class NetworkDialogsFacadeInterface
        : public NetworkFacadeInterface,
          public NetworkFacadeUsingEntityParser
{
public:
    NetworkDialogsFacadeInterface(const std::shared_ptr<EntityJsonParserInterface> &entityParser,
                                  const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    
    virtual Error getDialogs (std::vector<DialogEntity> &dialogs) = 0;
    virtual Error getPeerName(const DialogEntity::EntityId peerId, 
                              QString &peerName) = 0;
};

#endif // NETWORKDIALOGSFACADEINTERFACE_H
