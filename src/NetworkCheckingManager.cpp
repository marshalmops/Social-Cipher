#include "NetworkCheckingManager.h"

NetworkCheckingManager::NetworkCheckingManager(QThread *thread)
    : m_isChecking   {false},
      m_isRunning    {false},
      m_cycledChecker{nullptr},
      m_thread       {thread}
      //m_thread       {std::make_unique<QThread>()}
{
    if (thread) this->moveToThread(m_thread);
    
    QObject::connect(m_thread, &QThread::started, this, [&]() {
        this->launchCheckingCycle();
    });
}

NetworkCheckingManager::~NetworkCheckingManager()
{
    //m_thread->deleteLater();
    
    qInfo() << "NetworkCheckingManager deleting...";
}

void NetworkCheckingManager::run()
{
    m_thread->start();
}

void NetworkCheckingManager::stop()
{
    qInfo() << "stop() is called!";
    
    m_isRunning  = false;
    m_isChecking = false;
    
    m_thread->quit();
    this->deleteLater();
}

void NetworkCheckingManager::setCycledChecker(NetworkCycledCheckerInterface * const cycledChecker)
{
    m_cycledChecker = std::unique_ptr<NetworkCycledCheckerInterface>(cycledChecker);
}

void NetworkCheckingManager::startChecking()
{
    m_isChecking = true;
}

void NetworkCheckingManager::pauseChecking()
{
    m_isChecking = false;
    
    m_cycledChecker->abortCurrentCheck();
    
    qInfo() << "Current check aborting...";
}

void NetworkCheckingManager::launchCheckingCycle()
{
    auto *eventDispatcher = QThread::currentThread()->eventDispatcher();
    
    m_isRunning = true;
    
    while (m_isRunning) {
        eventDispatcher->processEvents(QEventLoop::AllEvents);
        
        if (m_isChecking) {
            std::vector<MessageEntity> newMessages;
        
            auto err = m_cycledChecker->check(newMessages);
            
            if (err.isValid()) 
                emit errorOccured(err);
            else
                if (!newMessages.empty()) 
                    emit newMessagesOccured(newMessages);
        }
    }
    
    emit execEnded();
    qInfo() << "Checking cycle has been ended!";
}
