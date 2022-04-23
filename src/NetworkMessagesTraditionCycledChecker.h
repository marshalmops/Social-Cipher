#ifndef NETWORKMESSAGESTRADITIONCYCLEDCHECKER_H
#define NETWORKMESSAGESTRADITIONCYCLEDCHECKER_H

#include <QHash>

#include "NetworkCycledCheckerInterface.h"
#include "NetworkDialogsFacadeInterface.h"

class NetworkMessagesTraditionCycledChecker : public NetworkCycledCheckerInterface
{
public:
    explicit NetworkMessagesTraditionCycledChecker(std::unique_ptr<NetworkDialogsFacadeInterface> &&dialogsFacade,
                                                   std::unique_ptr<NetworkDialogMessagesFacadeInterface> &&dialogMessagesFacade);

    virtual Error check(std::vector<MessageEntity> &newMessages) override;
    
//    virtual void prepare() override;
    
protected:
    Error initializeLastMessagesOfDialogs(const std::vector<DialogEntity> &dialogs);
    
    std::unique_ptr<NetworkDialogsFacadeInterface> m_dialogsFacade;
    
    QHash<DialogEntity::EntityId, MessageEntity::EntityId> m_lastMessagesOfDialogs;
};

#endif // NETWORKMESSAGESTRADITIONCYCLEDCHECKER_H
