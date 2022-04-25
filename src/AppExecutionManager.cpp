#include "AppExecutionManager.h"

AppExecutionManager::AppExecutionManager(QGuiApplication *app, 
                                         QObject *parent)
    : QObject           {parent},
      m_app             {app},
      m_quitingCounter  {0},
      m_isQuitingEnded  {false},
      m_isQuitingStarted{false}
{
    
}

void AppExecutionManager::processOccuredError(Error err)
{
    emit errorOccured(err);
}

void AppExecutionManager::processQuitRequest()
{
    if (m_isQuitingStarted) return;
    
    m_isQuitingStarted = true;
    
    qInfo() << "processQuitRequest() is called!";
    
    NetworkSettings::getInstance()->save();
    
    emit quittingRequested();
    
    auto *curEventLoop = QThread::currentThread()->eventDispatcher();
    
    while (!m_isQuitingEnded)
        curEventLoop->processEvents(QEventLoop::AllEvents);

    // onClosing is ended here... app event loop is stopped from this point...
}

void AppExecutionManager::moduleExecEnded()
{
    // checking condition...
    
    ++m_quitingCounter;
    
    if (m_quitingCounter != C_QUITING_POINTS_COUNT) return;
    
    m_isQuitingEnded = true;
    m_app->quit();
}

void AppExecutionManager::moveToLogin()
{
    emit openLoginView();
}

void AppExecutionManager::moveToDialogs()
{
    emit openDialogsView();
}

void AppExecutionManager::moveToDialogMessages()
{
    emit openDialogMessagesView();
}

void AppExecutionManager::startLoading()
{
    emit loadingStarted();
}

void AppExecutionManager::endLoading()
{
    emit loadingEnded();
}
