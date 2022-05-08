#include "NetworkDialogsFacadeInterface.h"

NetworkDialogsFacadeInterface::NetworkDialogsFacadeInterface(const std::shared_ptr<DialogJsonParserInterface> &dialogsParser,
                                                             const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : NetworkFacadeInterface{executor},
      m_dialogsParser       {dialogsParser}
{
    
}
