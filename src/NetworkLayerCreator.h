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

#include "MessageFilterVK.h"

#include "NetworkLoginFacadeVK.h"
#include "NetworkDialogsFacadeVK.h"
#include "NetworkDialogMessagesFacadeVK.h"

#include "NetworkRequestExecutorImpl.h"

#include "IncomingMessagesProcessorVK.h"

#include "AttachmentManagerVK.h"

class NetworkLayerCreator : public QObject
{
    Q_OBJECT
public:
    NetworkLayerCreator(NetworkCheckingManager *const checkingManager = nullptr);
    
signals:
    void errorOccured(Error error);
    
    void setCycledChecker(NetworkCycledCheckerInterface *cycledChecker);
    
    void setLoginModelNetworkInterface         (NetworkLoginFacadeInterface *loginFacade);
    void setDialogsModelNetworkInterface       (NetworkDialogsFacadeInterface *dialogsFacade,
                                                std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogMessagesFacade);
    void setDialogMessagesModelNetworkInterface(std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogMessagesFacade,
                                                std::shared_ptr<AttachmentManagerBase> attachmentsManager);
    
    void setAttachmentDefiner();
    
public slots:
    void createNetworkLayerForSocialNetwork(const NetworkSettings::SocialNetwork socialNetwork);
    
private:
    bool createLoginFacade   (const NetworkSettings::SocialNetwork socialNetwork,
                              const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                              std::unique_ptr<NetworkLoginFacadeInterface> &loginFacade);
    bool createDialogsFacade (const NetworkSettings::SocialNetwork socialNetwork,
                              const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                              const std::shared_ptr<DialogJsonParserInterface> &dialogsParser,
                              std::unique_ptr<NetworkDialogsFacadeInterface> &dialogsFacade);
    bool createMessagesFacade(const NetworkSettings::SocialNetwork socialNetwork,
                              const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                              std::unique_ptr<IncomingMessagesProcessorBase> &messagesProcessor,
                              std::unique_ptr<NetworkDialogMessagesFacadeInterface> &dialogMessagesFacade);
    bool createAttachmentsFacade(const NetworkSettings::SocialNetwork socialNetwork,
                                 const std::shared_ptr<AttachmentJsonParserInterface> &attachmentParser,
                                 const std::shared_ptr<NetworkRequestExecutorInterface> &executor,
                                 std::unique_ptr<NetworkAttachmentFacadeInterface> &attachmentsFacade);
    
    bool createMessagesProcessor(const NetworkSettings::SocialNetwork socialNetwork,
                                 const std::shared_ptr<AttachmentJsonParserInterface> &attachmentsParser,
                                 std::unique_ptr<IncomingMessagesProcessorBase> &messagesProcessor);
    
    bool createJsonDialogParser(const NetworkSettings::SocialNetwork socialNetwork,
                                std::shared_ptr<DialogJsonParserInterface> &dialogsParser);
    bool createJsonMessageParser(const NetworkSettings::SocialNetwork socialNetwork,
                                 const std::shared_ptr<AttachmentJsonParserInterface> &attachmentsParser,
                                 std::unique_ptr<MessageJsonParserInterface> &messagesParser);
    bool createJsonAttachmentParser(const NetworkSettings::SocialNetwork socialNetwork,
                                    std::shared_ptr<AttachmentJsonParserInterface> &attachmentsParser);
    
    bool createMessagesFilter(const NetworkSettings::SocialNetwork socialNetwork,
                              std::unique_ptr<MessageFilterInterface> &messagesFilter);
    
    bool createAttachmentsManager(const NetworkSettings::SocialNetwork socialNetwork,
                                  std::unique_ptr<NetworkAttachmentFacadeInterface> &attachmentsFacade,
                                  std::unique_ptr<AttachmentManagerBase> &attachmentManager);
    
    Error createCycledChecker (const NetworkSettings::SocialNetwork socialNetwork,
                               std::unique_ptr<NetworkCycledCheckerInterface> &cycledChecker);

    void setNetworkSettingsDefaults(const NetworkSettings::SocialNetwork socialNetwork);
    
    
    NetworkCheckingManager *m_checkingManager;
};

#endif // NETWORKLAYERCREATOR_H
