#include "NetworkLoginFacadeVK.h"


NetworkLoginFacadeVK::NetworkLoginFacadeVK(const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : NetworkLoginFacadeInterface{executor}
{
    
}

Error NetworkLoginFacadeVK::initializeWithToken(const QString token)
{
    auto err = Error{"Token checking error!"};
    
    if (token.isEmpty()) return err;
    
    auto networkSettings = NetworkSettings::getInstance();
    QString requestString;
    
    requestString += NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork()) + "/account.getProfileInfo?";
    requestString += QString("access_token=") + token + '&';
    requestString += QString("v=") + networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION);
    
    QJsonObject jsonReply;
    
    if (!m_requestExecutor->executeGetRequest(QUrl(requestString, QUrl::TolerantMode), jsonReply))
        return err;
    
    if (jsonReply.contains("error"))
        return Error{err.getText() + jsonReply["error"].toString()};
    
    err = Error{"Gotten login info validation is failed!", true};
    
    if (!jsonReply.contains("response"))   return err;
    if (!jsonReply["response"].isObject()) return err;
    
    jsonReply = jsonReply["response"].toObject();
    
    if (!jsonReply.contains("id")) return err;
    
    bool isConvOK{false};
    auto rawId = jsonReply["id"].toVariant().toLongLong(&isConvOK);
    
    if (!isConvOK) return err;
    
    networkSettings->setToken(token.toUtf8());
    networkSettings->setLocalPeerId(rawId);
    
    return Error{};
}

bool NetworkLoginFacadeVK::initialize()
{
    return true;
}
