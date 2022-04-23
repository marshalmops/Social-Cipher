#ifndef NETWORKLAYERCREATOR_H
#define NETWORKLAYERCREATOR_H

#include <QObject>

#include "NetworkCheckingManager.h"

#include "NetworkSettings.h"
#include "NetworkMessagesEventCycledChecker.h"
#include "NetworkMessagesTraditionCycledChecker.h"

#include "NetworkLoginFacadeInterface.h"
#include "NetworkDialogMessagesFacadeInterface.h"
#include "NetworkDialogsFacadeInterface.h"

#include "NetworkLoginFacadeVK.h"
#include "NetworkDialogsFacadeVK.h"
#include "NetworkDialogMessagesFacadeVK.h"

#include "NetworkRequestExecutorImpl.h"

class NetworkLayerCreator : public QObject
{
    Q_OBJECT
public:
    NetworkLayerCreator(NetworkCheckingManager *const checkingManager = nullptr);
    
signals:
    void errorOccured(Error error);
    
    void setCycledChecker(NetworkCycledCheckerInterface *cycledChecker);
    
    void setLoginModelFacades         (NetworkLoginFacadeInterface *loginFacade);
    void setDialogsModelFacades       (NetworkDialogsFacadeInterface *dialogsFacade,
                                       std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogMessagesFacade);
    void setDialogMessagesModelFacades(std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogMessagesFacade);
    
public slots:
    void createNetworkLayerForSocialNetwork(const NetworkSettings::SocialNetwork socialNetwork);
    
private:
    bool createLoginFacade   (const NetworkSettings::SocialNetwork socialNetwork,
                              const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                              std::unique_ptr<NetworkLoginFacadeInterface> &loginFacade);
    bool createJsonEntityParser(const NetworkSettings::SocialNetwork socialNetwork,
                                std::shared_ptr<EntityJsonParserInterface> &parser);
    bool createDialogsFacade (const NetworkSettings::SocialNetwork socialNetwork,
                              const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                              const std::shared_ptr<EntityJsonParserInterface> &parser,
                              std::unique_ptr<NetworkDialogsFacadeInterface> &dialogsFacade);
    bool createMessagesFacade(const NetworkSettings::SocialNetwork socialNetwork,
                              const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                              const std::shared_ptr<EntityJsonParserInterface> &parser,
                              std::unique_ptr<NetworkDialogMessagesFacadeInterface> &dialogMessagesFacade);
    
    Error createCycledChecker (const NetworkSettings::SocialNetwork socialNetwork,
                               std::unique_ptr<NetworkCycledCheckerInterface> &cycledChecker);

    void setNetworkSettingsDefaults(const NetworkSettings::SocialNetwork socialNetwork);
    
    
    NetworkCheckingManager *m_checkingManager;
};

#endif // NETWORKLAYERCREATOR_H
