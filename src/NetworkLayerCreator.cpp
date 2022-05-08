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
    std::shared_ptr<DialogJsonParserInterface>  dialogsParser;
    
    if (!createJsonDialogParser(socialNetwork, dialogsParser)) {
        emit errorOccured(Error{"Entity parser creating error!", true});
        
        return;
    }
    
    std::shared_ptr<AttachmentJsonParserInterface> attachmentsParser;
    
    if (!createJsonAttachmentParser(socialNetwork, attachmentsParser)) {
        emit errorOccured(Error{"Entity parser creating error!", true});
        
        return;
    }
    
    std::unique_ptr<IncomingMessagesProcessorBase> messagesProcessor;
    
    if (!createMessagesProcessor(socialNetwork, attachmentsParser, messagesProcessor)) {
        emit errorOccured(Error{"Messages processor creating error!", true});
        
        return;
    }
    
    std::unique_ptr<NetworkLoginFacadeInterface>          loginFacade;
    std::unique_ptr<NetworkDialogsFacadeInterface>        dialogsFacade;
    std::unique_ptr<NetworkDialogMessagesFacadeInterface> dialogMessagesFacade;
    std::unique_ptr<NetworkAttachmentFacadeInterface>     attachmentsFacade;
    
    if (!createLoginFacade(socialNetwork, executor, loginFacade)
     || !createDialogsFacade(socialNetwork, executor, dialogsParser, dialogsFacade)
     || !createMessagesFacade(socialNetwork, executor, messagesProcessor, dialogMessagesFacade)
     || !createAttachmentsFacade(socialNetwork, attachmentsParser, executor, attachmentsFacade))
    {
        emit errorOccured(Error{"Facades creating error!", true});
        
        return;
    }
    
    std::unique_ptr<AttachmentManagerBase> attachmentsManager;
    
    if (!createAttachmentsManager(socialNetwork, attachmentsFacade, attachmentsManager)) {
        emit errorOccured(Error{"Attachments creating error!", true});
        
        return;
    }
    
    std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogMessagesFacadeShared{dialogMessagesFacade.release()};
    std::shared_ptr<AttachmentManagerBase> attachmentManagerShared{attachmentsManager.release()};
    
    emit setLoginModelNetworkInterface(loginFacade.release());
    emit setDialogsModelNetworkInterface(dialogsFacade.release(), dialogMessagesFacadeShared);
    emit setDialogMessagesModelNetworkInterface(dialogMessagesFacadeShared, attachmentManagerShared);
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

bool NetworkLayerCreator::createDialogsFacade(const NetworkSettings::SocialNetwork socialNetwork,
                                              const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                                              const std::shared_ptr<DialogJsonParserInterface> &dialogsParser,
                                              std::unique_ptr<NetworkDialogsFacadeInterface> &dialogsFacade)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            dialogsFacade = std::make_unique<NetworkDialogsFacadeVK>(std::dynamic_pointer_cast<DialogJsonParserVK>(dialogsParser), executor);
            
            break;
        }
    }
    
    return true;
}

bool NetworkLayerCreator::createMessagesFacade(const NetworkSettings::SocialNetwork socialNetwork,
                                               const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                                               std::unique_ptr<IncomingMessagesProcessorBase> &messagesProcessor,
                                               std::unique_ptr<NetworkDialogMessagesFacadeInterface> &dialogMessagesFacade)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            dialogMessagesFacade = std::make_unique<NetworkDialogMessagesFacadeVK>(messagesProcessor, executor);
            
            break;
        }
    }
    
    return true;
}

bool NetworkLayerCreator::createAttachmentsFacade(const NetworkSettings::SocialNetwork socialNetwork,
                                                  const std::shared_ptr<AttachmentJsonParserInterface> &attachmentParser,
                                                  const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                                                  std::unique_ptr<NetworkAttachmentFacadeInterface> &attachmentsFacade)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            attachmentsFacade = std::make_unique<NetworkAttachmentFacadeVK>(attachmentParser, executor);
            
            break;
        }
    }
    
    return true;
}

bool NetworkLayerCreator::createMessagesProcessor(const NetworkSettings::SocialNetwork socialNetwork,
                                                  const std::shared_ptr<AttachmentJsonParserInterface> &attachmentsParser,
                                                  std::unique_ptr<IncomingMessagesProcessorBase> &messagesProcessor)
{
    std::unique_ptr<MessageFilterInterface> messagesFilter;
    
    if (!createMessagesFilter(socialNetwork, messagesFilter))
        return false;
    
    std::unique_ptr<MessageJsonParserInterface>    messagesParser;
//    std::shared_ptr<AttachmentJsonParserInterface> attachmentsParser;
    
    if (!createJsonMessageParser(socialNetwork, attachmentsParser, messagesParser))
        return false;
    
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            messagesProcessor = std::make_unique<IncomingMessagesProcessorBase>(messagesFilter, messagesParser);
            
            break;
        }
    }
    
    return true;
}

bool NetworkLayerCreator::createJsonDialogParser(const NetworkSettings::SocialNetwork socialNetwork,
                                                 std::shared_ptr<DialogJsonParserInterface> &dialogsParser)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            dialogsParser  = std::make_shared<DialogJsonParserVK>();
            
            break;
        }
    }
    
    return true;
}

bool NetworkLayerCreator::createJsonMessageParser(const NetworkSettings::SocialNetwork socialNetwork,
                                                  const std::shared_ptr<AttachmentJsonParserInterface> &attachmentsParser,
                                                  std::unique_ptr<MessageJsonParserInterface> &messagesParser)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            messagesParser = std::make_unique<MessageJsonParserVK>(attachmentsParser);
            
            break;
        }
    }
    
    return true;
}

bool NetworkLayerCreator::createJsonAttachmentParser(const NetworkSettings::SocialNetwork socialNetwork, 
                                                     std::shared_ptr<AttachmentJsonParserInterface> &attachmentsParser)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            attachmentsParser = std::make_shared<AttachmentJsonParserVK>();
            
            break;
        }
    }
    
    return true;
}

bool NetworkLayerCreator::createMessagesFilter(const NetworkSettings::SocialNetwork socialNetwork,
                                               std::unique_ptr<MessageFilterInterface> &messagesFilter)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            messagesFilter = std::make_unique<MessageFilterVK>();
            
            break;
        }
    }
    
    return true;
}

bool NetworkLayerCreator::createAttachmentsManager(const NetworkSettings::SocialNetwork socialNetwork,
                                                   std::unique_ptr<NetworkAttachmentFacadeInterface> &attachmentsFacade,
                                                   std::unique_ptr<AttachmentManagerBase> &attachmentManager)
{
    switch (socialNetwork) {
        case NetworkSettings::SocialNetwork::SNT_VK: {
            attachmentManager = std::make_unique<AttachmentManagerVK>(attachmentsFacade);
            
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
    std::shared_ptr<DialogJsonParserInterface>  dialogsParser;
    std::shared_ptr<AttachmentJsonParserInterface> attachmentsParser;
    
    if (!createJsonDialogParser(socialNetwork, dialogsParser)
     || !createJsonAttachmentParser(socialNetwork, attachmentsParser))
        return Error{"Entity parser creating error!", true};
    
    std::unique_ptr<IncomingMessagesProcessorBase> messagesProcessor;
    
    if (!createMessagesProcessor(socialNetwork, attachmentsParser, messagesProcessor))
        return Error{"Messages processor creating error!", true};
    
    std::unique_ptr<NetworkDialogMessagesFacadeInterface> messagesFacade;
    
    if (!createMessagesFacade(socialNetwork, executor, messagesProcessor, messagesFacade))
        return Error{"Messages facade creating error!", true};
    
    switch (checkingType) {
        case NetworkSettings::CheckingType::CT_EVENTIONAL: {
            cycledChecker = std::make_unique<NetworkMessagesEventCycledChecker>(std::move(messagesFacade));
            
            break;
        }
        case NetworkSettings::CheckingType::CT_TRADITIONAL: {
            std::unique_ptr<NetworkDialogsFacadeInterface> dialogsFacade;
            
            if (!createDialogsFacade(socialNetwork, executor, dialogsParser, dialogsFacade))
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
