#ifndef LOGINMODEL_H
#define LOGINMODEL_H

#include <QObject>

#include "Error.h"
#include "NetworkLoginFacadeInterface.h"

class LoginModel : public QObject
{
    Q_OBJECT
public:
    LoginModel(/*std::unique_ptr<NetworkLoginFacadeInterface> &&loginFacade,*/
               QObject *parent = nullptr);
    
signals:
    void errorOccured(const Error error);
    
    void signedIn();
    
    void socialNetworkChanged(const NetworkSettings::SocialNetwork socialNetwork);
    
public slots:
    void setLoginModelNetworkInterface(NetworkLoginFacadeInterface *loginFacade);
    
    void signIn();  // qml.
    void signInWithToken(const QString token);  // qml.
    
    void changeSocialNetwork(const NetworkSettings::SocialNetwork socialNetwork);   // qml.
    
private:
    std::unique_ptr<NetworkLoginFacadeInterface> m_loginFacade;
};

#endif // LOGINMODEL_H
