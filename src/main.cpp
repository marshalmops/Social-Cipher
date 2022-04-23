#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "AppInitializer.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    
    QGuiApplication       app(argc, argv);
    
    QCoreApplication::setOrganizationName("");
    QCoreApplication::setApplicationName("SocialCipher");
    
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    
    qmlRegisterUncreatableType<Error>("com.SocialCipher.Entities", 1, 0, "error", "");
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, 
                     [url](QObject *obj, const QUrl &objUrl) {
                        if (!obj && url == objUrl) QCoreApplication::exit(-1);
                     },
                     Qt::QueuedConnection);
    
    NetworkCheckingManager *networkCheckingManager; 
    
    std::unique_ptr<NetworkLayerCreator>    networkLayerCreator;
    
    std::unique_ptr<AppExecutionManager>    appExecManager;
    std::unique_ptr<LoginModel>             loginModel;
    std::unique_ptr<DialogsModel>           dialogsModel;
    std::unique_ptr<DialogMessagesModel>    dialogMessagesModel;
    
    bool autoSigningInFlag{false};
    
    bool initFlag = AppInitializer::initializeApp(&app,
                                                  appExecManager,
                                                  loginModel,
                                                  dialogsModel,
                                                  dialogMessagesModel,
                                                  networkCheckingManager,
                                                  networkLayerCreator,
                                                  autoSigningInFlag);
    
    auto *rootContext = engine.rootContext();
    
    rootContext->setContextProperty("appExecManager",      appExecManager.get());
    rootContext->setContextProperty("loginModel",          loginModel.get());
    rootContext->setContextProperty("dialogsModel",        dialogsModel.get());
    rootContext->setContextProperty("dialogMessagesModel", dialogMessagesModel.get());
    
    engine.load(url);
    
    if (!initFlag)
        appExecManager->processOccuredError(Error{"Initialization error!", true});
    
    if (autoSigningInFlag) {
        appExecManager->startLoading();
        
        loginModel->signInWithToken(NetworkSettings::getInstance()->getToken());
    }
        
    return app.exec();
}
