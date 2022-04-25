#ifndef NETWORKCYCLEDCHECKERINTERFACE_H
#define NETWORKCYCLEDCHECKERINTERFACE_H

#include <memory>

#include "NetworkDialogMessagesFacadeInterface.h"
#include "Error.h"

class NetworkCycledCheckerInterface
{
public:
    explicit NetworkCycledCheckerInterface(std::unique_ptr<NetworkDialogMessagesFacadeInterface> &&dialogMessagesFacade);
    
    virtual Error check(std::vector<MessageEntity> &newMessages) = 0;
    
    void abortCurrentCheck();
    
//    virtual void prepare() = 0;
    
protected:
    std::unique_ptr<NetworkDialogMessagesFacadeInterface> m_dialogMessagesFacade;
};

#endif // NETWORKCYCLEDCHECKERINTERFACE_H
