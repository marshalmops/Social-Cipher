#include "AppInitializer.h"


bool AppInitializer::initializeApp(QGuiApplication                         *app,
                                   std::unique_ptr<AppExecutionManager>    &appExecManager,
                                   std::unique_ptr<LoginModel>             &loginModel,
                                   std::unique_ptr<DialogsModel>           &dialogsModel,
                                   std::unique_ptr<DialogMessagesModel>    &dialogMessagesModel,
                                   NetworkCheckingManager                 *&networkCheckingManager, 
                                   std::unique_ptr<NetworkLayerCreator>    &networkLayerCreator,
                                   bool                                    &autoSigningInFlag)
{
    appExecManager = std::make_unique<AppExecutionManager>(app);
    
    auto networkSettings = NetworkSettings::getInstance();
    
    if (!(autoSigningInFlag = networkSettings->load()))
        networkSettings->setDefaults();
    
    // creating models:
    
    loginModel          = std::make_unique<LoginModel>();
    dialogsModel        = std::make_unique<DialogsModel>();
    dialogMessagesModel = std::make_unique<DialogMessagesModel>(std::make_unique<RSAEncoder>(), std::make_unique<AESEncoder>());
    
    // cycled checker manager creation:
    
    QThread *checkingThread = new QThread(app);
    
    networkCheckingManager = new NetworkCheckingManager(checkingThread/*dialogsCycledChecker,*/);
    
    // NetworkLayerCreator creating...
    
    networkLayerCreator = std::make_unique<NetworkLayerCreator>(networkCheckingManager);
    
    // signals / slots bindings:
    
    QObject::connect(appExecManager.get(), &AppExecutionManager::quittingRequested, networkCheckingManager, &NetworkCheckingManager::stop, Qt::QueuedConnection);
    QObject::connect(appExecManager.get(), &AppExecutionManager::quittingRequested, dialogsModel.get(),     &DialogsModel::resetDialogs);
    
    QObject::connect(loginModel.get(), &LoginModel::errorOccured,         appExecManager.get(),         &AppExecutionManager::processOccuredError);
    QObject::connect(loginModel.get(), &LoginModel::signedIn,             dialogsModel.get(),           &DialogsModel::initializeDialogs);    
    QObject::connect(loginModel.get(), &LoginModel::socialNetworkChanged, networkLayerCreator.get(),    &NetworkLayerCreator::createNetworkLayerForSocialNetwork);
    
    QObject::connect(dialogsModel.get(), &DialogsModel::errorOccured,                appExecManager.get(),         &AppExecutionManager::processOccuredError);
    QObject::connect(dialogsModel.get(), &DialogsModel::dialogSet,                   dialogMessagesModel.get(),    &DialogMessagesModel::setDialogMessages);
    QObject::connect(dialogsModel.get(), &DialogsModel::activeDialogMessageReceived, dialogMessagesModel.get(),    &DialogMessagesModel::appendMessage);
    QObject::connect(dialogsModel.get(), &DialogsModel::dialogsInitialized,          networkCheckingManager,       &NetworkCheckingManager::startChecking, Qt::QueuedConnection);
    QObject::connect(dialogsModel.get(), &DialogsModel::dialogsUnset,                networkCheckingManager,       &NetworkCheckingManager::pauseChecking, Qt::QueuedConnection);
    QObject::connect(dialogsModel.get(), &DialogsModel::dialogsInitialized,          appExecManager.get(),         &AppExecutionManager::endLoading);  
    QObject::connect(dialogsModel.get(), &DialogsModel::dialogsInitialized,          appExecManager.get(),         &AppExecutionManager::moveToDialogs);  
    QObject::connect(dialogsModel.get(), &DialogsModel::dialogsUnset,                appExecManager.get(),         &AppExecutionManager::moveToLogin);
    QObject::connect(dialogsModel.get(), &DialogsModel::dialogsReset,                appExecManager.get(),         &AppExecutionManager::moduleExecEnded);
    
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::errorOccured,        appExecManager.get(), &AppExecutionManager::processOccuredError);
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::messagesSet,         appExecManager.get(), &AppExecutionManager::moveToDialogMessages);
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::messagesSet,         appExecManager.get(), &AppExecutionManager::endLoading);
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::messagesUnset,       appExecManager.get(), &AppExecutionManager::moveToDialogs);
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::messagesUnset,       dialogsModel.get(),   &DialogsModel::closeDialog);    
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::messageRowInserted,  dialogsModel.get(),   &DialogsModel::currentDialogMessageInserted);
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::messageSent,         appExecManager.get(), &AppExecutionManager::endLoading);
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::encryptionInitiated, appExecManager.get(), &AppExecutionManager::startLoading);
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::encryptionCanceled,  appExecManager.get(), &AppExecutionManager::endLoading);
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::encryptionStarted,   appExecManager.get(), &AppExecutionManager::endLoading);    
    
    QObject::connect(networkCheckingManager, &NetworkCheckingManager::newMessagesOccured, dialogsModel.get(),   &DialogsModel::newMessagesOccured,         Qt::QueuedConnection);
    QObject::connect(networkCheckingManager, &NetworkCheckingManager::errorOccured,       appExecManager.get(), &AppExecutionManager::processOccuredError, Qt::QueuedConnection);
    QObject::connect(networkCheckingManager, &NetworkCheckingManager::execEnded,          appExecManager.get(), &AppExecutionManager::moduleExecEnded,     Qt::QueuedConnection);    
    
    QObject::connect(networkLayerCreator.get(), &NetworkLayerCreator::setCycledChecker,                       networkCheckingManager,       &NetworkCheckingManager::setCycledChecker, Qt::QueuedConnection);
    QObject::connect(networkLayerCreator.get(), &NetworkLayerCreator::setLoginModelNetworkInterface,          loginModel.get(),             &LoginModel::setLoginModelNetworkInterface);
    QObject::connect(networkLayerCreator.get(), &NetworkLayerCreator::setDialogsModelNetworkInterface,        dialogsModel.get(),           &DialogsModel::setDialogsModelNetworkInterface);
    QObject::connect(networkLayerCreator.get(), &NetworkLayerCreator::setDialogMessagesModelNetworkInterface, dialogMessagesModel.get(),    &DialogMessagesModel::setDialogMessagesModelNetworkInterface);
    
    // initial network layer setting:
    
    networkLayerCreator->createNetworkLayerForSocialNetwork(networkSettings->getSocialNetwork());
    
    networkCheckingManager->run();
    
    return true;
}
