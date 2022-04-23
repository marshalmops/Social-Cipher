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
    
    loginModel          = std::make_unique<LoginModel>(/*std::unique_ptr<NetworkLoginFacadeInterface>(networkLoginFacade.release())*/);
    dialogsModel        = std::make_unique<DialogsModel>(/*std::unique_ptr<NetworkDialogsFacadeInterface>(networkDialogsFacadeForModel.release())*/);
    dialogMessagesModel = std::make_unique<DialogMessagesModel>(std::make_unique<RSAEncoder>() /*std::unique_ptr<NetworkDialogMessagesFacadeInterface>(networkDialogMessagesFacadeForModel.release())*/);
    
    // cycled checker manager creation:
    
    QThread *checkingThread = new QThread(app);
    
    networkCheckingManager = new NetworkCheckingManager(checkingThread/*dialogsCycledChecker,*/);
    
    // NetworkLayerCreator creating...
    
    networkLayerCreator = std::make_unique<NetworkLayerCreator>(networkCheckingManager);
    
    // signals / slots bindings:
    
    QObject::connect(appExecManager.get(), &AppExecutionManager::quittingRequested, networkCheckingManager, &NetworkCheckingManager::stop, Qt::QueuedConnection);
    
    
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
    
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::errorOccured,  appExecManager.get(),    &AppExecutionManager::processOccuredError);
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::messagesSet,   appExecManager.get(),    &AppExecutionManager::moveToDialogMessages);
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::messagesUnset, appExecManager.get(),    &AppExecutionManager::moveToDialogs);
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::messagesUnset, dialogsModel.get(),      &DialogsModel::closeDialog);    
    QObject::connect(dialogMessagesModel.get(), &DialogMessagesModel::messageRowInserted, dialogsModel.get(), &DialogsModel::currentDialogMessageInserted);
    
    QObject::connect(networkCheckingManager, &NetworkCheckingManager::newMessagesOccured, dialogsModel.get(),   &DialogsModel::newMessagesOccured,         Qt::QueuedConnection);
    QObject::connect(networkCheckingManager, &NetworkCheckingManager::errorOccured,       appExecManager.get(), &AppExecutionManager::processOccuredError, Qt::QueuedConnection);
    QObject::connect(networkCheckingManager, &NetworkCheckingManager::execEnded,          appExecManager.get(), &AppExecutionManager::moduleExecEnded,     Qt::QueuedConnection);    
    
    QObject::connect(networkLayerCreator.get(), &NetworkLayerCreator::setCycledChecker,              networkCheckingManager, &NetworkCheckingManager::setCycledChecker, Qt::QueuedConnection);
    QObject::connect(networkLayerCreator.get(), &NetworkLayerCreator::setLoginModelFacades,          loginModel.get(),             &LoginModel::setLoginModelFacades);
    QObject::connect(networkLayerCreator.get(), &NetworkLayerCreator::setDialogsModelFacades,        dialogsModel.get(),           &DialogsModel::setDialogsModelFacades);
    QObject::connect(networkLayerCreator.get(), &NetworkLayerCreator::setDialogMessagesModelFacades, dialogMessagesModel.get(),    &DialogMessagesModel::setDialogMessagesModelFacades);
    
    // initial network layer setting:
    
    networkLayerCreator->createNetworkLayerForSocialNetwork(networkSettings->getSocialNetwork());
    
    networkCheckingManager->run();
    
    return true;
}
