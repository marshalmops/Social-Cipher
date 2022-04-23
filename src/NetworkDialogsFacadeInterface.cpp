#include "NetworkDialogsFacadeInterface.h"

NetworkDialogsFacadeInterface::NetworkDialogsFacadeInterface(const std::shared_ptr<EntityJsonParserInterface> &entityParser,
                                                             const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : NetworkFacadeInterface        {executor},
      NetworkFacadeUsingEntityParser{entityParser}
{
    
}
