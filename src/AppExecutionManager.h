#ifndef APPEXECUTIONMANAGER_H
#define APPEXECUTIONMANAGER_H

#include <QObject>
#include <QGuiApplication>
#include <QAbstractEventDispatcher>

#include "Error.h"

#include "NetworkSettings.h"

class AppExecutionManager : public QObject
{
    Q_OBJECT
    
    constexpr static const uint8_t C_QUITING_POINTS_COUNT = 2;
    
public:
    AppExecutionManager(QGuiApplication *app,
                        QObject *parent = nullptr);
    
public slots:
    void processOccuredError(Error err);
    void processQuitRequest();
    
    void moduleExecEnded();
    
    void moveToLogin();
    void moveToDialogs();
    void moveToDialogMessages();
    
    void startLoading();
    void endLoading  ();
    
signals:
    void errorOccured(Error err);
    
    void openLoginView();
    void openDialogsView();
    void openDialogMessagesView();
    
    void loadingStarted();
    void loadingEnded  ();
    
    void quittingRequested();
    
private:
    QGuiApplication *m_app;
    
    bool    m_isQuitingStarted;
    bool    m_isQuitingEnded;
    uint8_t m_quitingCounter;
};

#endif // APPEXECUTIONMANAGER_H
