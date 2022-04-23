#ifndef NETWORKMESSAGESEVENTCYCLEDCHECKER_H
#define NETWORKMESSAGESEVENTCYCLEDCHECKER_H

#include "NetworkCycledCheckerInterface.h"

class NetworkMessagesEventCycledChecker : public NetworkCycledCheckerInterface
{
public:
    NetworkMessagesEventCycledChecker(std::unique_ptr<NetworkDialogMessagesFacadeInterface> &&dialogMessagesFacade);
    
//public slots:
//    void startMessagesCycle(const EntityInterface::EntityId peerId);
//    void pauseMessagesCycle();
    
    virtual Error check(std::vector<MessageEntity> &newMessages) override;
//    virtual void prepare() override;
    
//    void setPeerId(const EntityInterface::EntityId peerId);
    
    
private:
//    void exec();
    
//    bool m_isProcessing;
    
//    EntityInterface::EntityId m_peerId;
    
//    std::unique_ptr<NetworkDialogMessagesFacadeInterface> m_dialogMessagesFacade;
};

#endif // NETWORKMESSAGESEVENTCYCLEDCHECKER_H
