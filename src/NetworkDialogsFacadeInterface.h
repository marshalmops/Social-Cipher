#ifndef NETWORKDIALOGSFACADEINTERFACE_H
#define NETWORKDIALOGSFACADEINTERFACE_H

#include "NetworkFacadeInterface.h"
//#include "NetworkFacadeUsingEntityParser.h"
#include "DialogJsonParserInterface.h"

#include "DialogEntityBase.h"

class NetworkDialogsFacadeInterface
        : public NetworkFacadeInterface/*,
          public NetworkFacadeUsingEntityParser*/
{
public:
    NetworkDialogsFacadeInterface(const std::shared_ptr<DialogJsonParserInterface> &dialogsParser,
                                  const std::shared_ptr<NetworkRequestExecutorInterface> &executor);
    
    virtual Error getDialogs (std::vector<std::unique_ptr<DialogEntityBase>> &dialogs) = 0;
    virtual Error getPeerName(const DialogEntityBase::EntityId peerId, 
                              QString &peerName) = 0;
    
protected:
    std::shared_ptr<DialogJsonParserInterface> m_dialogsParser;
};

#endif // NETWORKDIALOGSFACADEINTERFACE_H
