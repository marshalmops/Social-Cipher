#ifndef APPINITIALIZER_H
#define APPINITIALIZER_H

#include <memory>
#include <QFile>

#include "AppExecutionManager.h"

#include "LoginModel.h"
#include "DialogsModel.h"
#include "DialogMessagesModel.h"

#include "NetworkCheckingManager.h"

#include "NetworkSettings.h"
#include "NetworkLayerCreator.h"

#include "RSAEncoder.h"
#include "AESEncoder.h"

class AppInitializer
{
public:
    AppInitializer() = delete;
    
    static bool initializeApp(QGuiApplication                         *app,
                              std::unique_ptr<AppExecutionManager>    &appExecManager,
                              std::unique_ptr<LoginModel>             &loginModel,
                              std::unique_ptr<DialogsModel>           &dialogsModel,
                              std::unique_ptr<DialogMessagesModel>    &dialogMessagesModel,
                              NetworkCheckingManager                 *&networkCheckingManager,
                              std::unique_ptr<NetworkLayerCreator>    &networkLayerCreator,
                              bool                                    &autoSigningInFlag);
};

#endif // APPINITIALIZER_H
