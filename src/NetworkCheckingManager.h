#ifndef NETWORKCHECKINGMANAGER_H
#define NETWORKCHECKINGMANAGER_H

#include <QEventLoop>
#include <QObject>
#include <QThread>
#include <QAbstractEventDispatcher>

#include "NetworkCycledCheckerInterface.h"

class NetworkCheckingManager : public QObject
{
    Q_OBJECT
public:
    NetworkCheckingManager(QThread *thread);
    ~NetworkCheckingManager();
    
public slots:
    void run ();
    void stop();
    
    void setCycledChecker(NetworkCycledCheckerInterface *cycledChecker);
    
    void startChecking();
    void pauseChecking();
    
signals:
    void errorOccured      (Error error);
    void newMessagesOccured(std::vector<std::shared_ptr<MessageEntityBase>> messages);
    
    void execEnded();
    
private:
    void launchCheckingCycle();
    
    bool m_isChecking;
    bool m_isRunning;
    
    std::unique_ptr<NetworkCycledCheckerInterface> m_cycledChecker;
    
    QThread* m_thread;
};

#endif // NETWORKCHECKINGMANAGER_H
