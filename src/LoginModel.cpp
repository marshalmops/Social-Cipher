#include "LoginModel.h"

LoginModel::LoginModel(QObject *parent)
    : QObject{parent}
{
    
}

void LoginModel::signIn()
{
    // signing in without a token...
}

void LoginModel::signInWithToken(const QString token)
{
    auto err = m_loginFacade->initializeWithToken(token);
    
    if (err.isValid()) {
        emit errorOccured(err);
        
        return;
    }
    
    emit signedIn();
}

void LoginModel::changeSocialNetwork(const NetworkSettings::SocialNetwork socialNetwork)
{
    emit socialNetworkChanged(socialNetwork);
}

void LoginModel::setLoginModelNetworkInterface(NetworkLoginFacadeInterface *loginFacade)
{
    m_loginFacade = std::unique_ptr<NetworkLoginFacadeInterface>(loginFacade);
    
    if (!m_loginFacade->initialize()) {
        emit errorOccured(Error{"Login facade init error!", true});
    }
}
