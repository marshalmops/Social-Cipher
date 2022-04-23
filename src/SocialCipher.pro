QT += quick network

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#win32-g++ {
#    INCLUDEPATH += C:\boost_1_77_0\boost_mingw_810\include\boost-1_77
#    LIBS += -LC:\boost_1_77_0\boost_mingw_810\lib
#    INCLUDEPATH += C:\OpenSSL-Win64\include
#    LIBS += -LC:\OpenSSL-Win64\lib -llibcrypto
#}

#win32:LIBS += -lsetupapi
#win32:LIBS += -lwsock32
#win32:LIBS += -lws2_32
#win32:DEFINES += _WIN32_WINNT=0x0501

#INCLUDEPATH += C:\Users\Admin\Documents\SocialCipher\rsa\lib

SOURCES += \
        AppExecutionManager.cpp \
        AppInitializer.cpp \
        CipherKey.cpp \
        DialogEntity.cpp \
        DialogMessagesModel.cpp \
        DialogsModel.cpp \
        EncoderInterface.cpp \
        EntityInterface.cpp \
        EntityJsonParserInterface.cpp \
        EntityJsonParserVK.cpp \
        Error.cpp \
        LoginModel.cpp \
        MessageEntity.cpp \
        NetworkCheckingManager.cpp \
        NetworkCycledCheckerInterface.cpp \
        NetworkDialogMessagesFacadeInterface.cpp \
        NetworkDialogMessagesFacadeVK.cpp \
        NetworkDialogsFacadeInterface.cpp \
        NetworkDialogsFacadeVK.cpp \
        NetworkFacadeInterface.cpp \
        NetworkFacadeUsingEntityParser.cpp \
        NetworkFacadeVK.cpp \
        NetworkLayerCreator.cpp \
        NetworkLoginFacadeInterface.cpp \
        NetworkLoginFacadeVK.cpp \
        NetworkMessagesEventCycledChecker.cpp \
        NetworkMessagesTraditionCycledChecker.cpp \
        NetworkRequestExecutorImpl.cpp \
        NetworkRequestExecutorInterface.cpp \
        NetworkSettings.cpp \
        RSAEncoder.cpp \
        SettingsBase.cpp \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    AppExecutionManager.h \
    AppInitializer.h \
    CipherKey.h \
    DialogEntity.h \
    DialogMessagesModel.h \
    DialogsModel.h \
    EncoderInterface.h \
    EntityInterface.h \
    EntityJsonParserInterface.h \
    EntityJsonParserVK.h \
    Error.h \
    LoginModel.h \
    MessageEntity.h \
    NetworkCheckingManager.h \
    NetworkCycledCheckerInterface.h \
    NetworkDialogMessagesFacadeInterface.h \
    NetworkDialogMessagesFacadeVK.h \
    NetworkDialogsFacadeInterface.h \
    NetworkDialogsFacadeVK.h \
    NetworkFacadeInterface.h \
    NetworkFacadeUsingEntityParser.h \
    NetworkFacadeVK.h \
    NetworkLayerCreator.h \
    NetworkLoginFacadeInterface.h \
    NetworkLoginFacadeVK.h \
    NetworkMessagesEventCycledChecker.h \
    NetworkMessagesTraditionCycledChecker.h \
    NetworkRequestExecutorImpl.h \
    NetworkRequestExecutorInterface.h \
    NetworkSettings.h \
    RSAEncoder.h \
    SettingsBase.h

include($$PWD/../Qt-Secret/src/Qt-Secret.pri)
