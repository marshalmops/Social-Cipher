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
        return (m_lastError = err);
    
    if (jsonReply.contains("error"))
        return (m_lastError = Error{err.getText() + jsonReply["error"].toString()});
    
    err = Error{"Gotten login info validation is failed!", true};
    
    if (!jsonReply.contains("response"))   return (m_lastError = err);
    if (!jsonReply["response"].isObject()) return (m_lastError = err);
    
    jsonReply = jsonReply["response"].toObject();
    
    if (!jsonReply.contains("id")) return (m_lastError = err);
    
    bool isConvOK{false};
    auto rawId = jsonReply["id"].toVariant().toLongLong(&isConvOK);
    
    if (!isConvOK) return (m_lastError = err);
    
    networkSettings->setToken(token.toUtf8());
    networkSettings->setLocalPeerId(rawId);
    
    return Error{};
}

bool NetworkLoginFacadeVK::initialize()
{
    return true;
}
