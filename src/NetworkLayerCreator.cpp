#include "NetworkLayerCreator.h"

NetworkLayerCreator::NetworkLayerCreator(NetworkCheckingManager * const checkingManager)
    : m_checkingManager{checkingManager}
{
    
}

void NetworkLayerCreator::createNetworkLayerForSocialNetwork(const NetworkSettings::SocialNetwork socialNetwork)
{
    auto networkSettings = NetworkSettings::getInstance();
    
    if (socialNetwork == NetworkSettings::SocialNetwork::SNT_INVALID)
        return;
    if (socialNetwork == networkSettings->getSocialNetwork()) {
        if (networkSettings->isNetworkLayerCreated()) return;
        else networkSettings->setNetworkLayerCreatedFlag();
    }
    
    setNetworkSettingsDefaults(socialNetwork);
    
    std::unique_ptr<NetworkCycledCheckerInterface> cycledChecker;
    
    auto err = createCycledChecker(socialNetwork, cycledChecker);
    
    if (err.isValid()) {
        emit errorOccured(err);
        
        return;
    }
    
    // models facades creation...
    
    std::shared_ptr<NetworkRequestExecutorImpl> executor = std::make_shared<NetworkRequestExecutorImpl>();
    std::shared_ptr<EntityJsonParserInterface>  parser;
    
    if (!createJsonEntityParser(socialNetwork, parser)) {
        emit errorOccured(Error{"Entity parser creating error!", true});
        
        return;
    }
    
    std::unique_ptr<NetworkLoginFacadeInterface>          loginFacade;
    std::unique_ptr<NetworkDialogsFacadeInterface>        dialogsFacade;
    std::unique_ptr<NetworkDialogMessagesFacadeInterface> dialogMessagesFacade;
    
    if (!createLoginFacade(socialNetwork, executor, loginFacade)
     || !createDialogsFacade(socialNetwork, executor, parser, dialogsFacade)
     || !createMessagesFacade(socialNetwork, executor, parser, dialogMessagesFacade))
    {
        emit errorOccured(Error{"Facades creating error!", true});
        
        return;
    }
    
    std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogMessagesFacadeShared{dialogMessagesFacade.release()};
    
    emit setLoginModelFacades(loginFacade.release());
    emit setDialogsModelFacades(dialogsFacade.release(), dialogMessagesFacadeShared);
    emit setDialogMessagesModelFacades(dialogMessagesFacadeShared);
    emit setCycledChecker(cycledChecker.release());
}

bool NetworkLayerCreator::createLoginFacade(const NetworkSettings::SocialNetwork socialNetwork,
                                            const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                                            std::unique_ptr<NetworkLoginFacadeInterface> &loginFacade)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            loginFacade = std::make_unique<NetworkLoginFacadeVK>(executor);
            
            break;
        }
    }
    
    return true;
}

bool NetworkLayerCreator::createJsonEntityParser(const NetworkSettings::SocialNetwork socialNetwork,
                                                 std::shared_ptr<EntityJsonParserInterface> &parser)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            parser = std::make_shared<EntityJsonParserVK>();
            
            break;
        }
    }
    
    return true;
}

bool NetworkLayerCreator::createDialogsFacade(const NetworkSettings::SocialNetwork socialNetwork,
                                              const std::shared_ptr<NetworkRequestExecutorInterface> &executor, 
                                              const std::shared_ptr<EntityJsonParserInterface> &parser,
                                              std::unique_ptr<NetworkDialogsFacadeInterface> &dialogsFacade)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            dialogsFacade = std::make_unique<NetworkDialogsFacadeVK>(std::dynamic_pointer_cast<EntityJsonParserVK>(parser), executor);
            
            break;
        }
    }
    
    return true;
}

bool NetworkLayerCreator::createMessagesFacade(const NetworkSettings::SocialNetwork socialNetwork,
                                               const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                                               const std::shared_ptr<EntityJsonParserInterface> &parser,
                                               std::unique_ptr<NetworkDialogMessagesFacadeInterface> &dialogMessagesFacade)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            dialogMessagesFacade = std::make_unique<NetworkDialogMessagesFacadeVK>(std::dynamic_pointer_cast<EntityJsonParserVK>(parser), executor);
            
            break;
        }
    }
    
    return true;
}

Error NetworkLayerCreator::createCycledChecker(const NetworkSettings::SocialNetwork socialNetwork, 
                                               std::unique_ptr<NetworkCycledCheckerInterface> &cycledChecker)
{
    auto checkingType = NetworkSettings::getCheckingTypeBySocialNetwork(socialNetwork);
    
    if (checkingType == NetworkSettings::CheckingType::CT_INVALID)
        return Error{"Social network type is invalid!", true};
    
    std::shared_ptr<NetworkRequestExecutorImpl> executor = std::make_shared<NetworkRequestExecutorImpl>(m_checkingManager);
    std::shared_ptr<EntityJsonParserInterface>  parser;
    
    if (!createJsonEntityParser(socialNetwork, parser))
        return Error{"Entity parser creating error!", true};
    
    std::unique_ptr<NetworkDialogMessagesFacadeInterface> messagesFacade;
    
    if (!createMessagesFacade(socialNetwork, executor, parser, messagesFacade))
        return Error{"Messages facade creating error!", true};
    
    switch (checkingType) {
        case NetworkSettings::CheckingType::CT_EVENTIONAL: {
            cycledChecker = std::make_unique<NetworkMessagesEventCycledChecker>(std::move(messagesFacade));
            
            break;
        }
        case NetworkSettings::CheckingType::CT_TRADITIONAL: {
            std::unique_ptr<NetworkDialogsFacadeInterface> dialogsFacade;
            
            if (!createDialogsFacade(socialNetwork, executor, parser, dialogsFacade))
                return Error{"Dialogs facade creating error!", true};
            
            cycledChecker = std::make_unique<NetworkMessagesTraditionCycledChecker>(std::move(dialogsFacade), std::move(messagesFacade));
            
            break;
        }
    }
        
    return Error{};
}

void NetworkLayerCreator::setNetworkSettingsDefaults(const NetworkSettings::SocialNetwork socialNetwork)
{
    auto networkSettings = NetworkSettings::getInstance();
    
    networkSettings->clearAdditionalProps();
    
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            networkSettings->setAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_WAIT_PROP_NAME,    "25");
            networkSettings->setAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_MODE_PROP_NAME,    "2");
            networkSettings->setAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_VERSION_PROP_NAME, "3");
            networkSettings->setAdditionalPropValue(NetworkFacadeVK::C_API_VERSION,                "5.131");
            
            break;
        }
    }
}
