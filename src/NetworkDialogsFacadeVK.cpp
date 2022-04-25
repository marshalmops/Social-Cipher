#include "NetworkDialogsFacadeVK.h"

NetworkDialogsFacadeVK::NetworkDialogsFacadeVK(const std::shared_ptr<EntityJsonParserVK> &parser,
                                               const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : NetworkDialogsFacadeInterface{parser, executor}
{
    
}

Error NetworkDialogsFacadeVK::getDialogs(std::vector<DialogEntity> &dialogs)
{
    auto networkSettings = NetworkSettings::getInstance();
    QString requestString{};
    
    requestString += NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork()) + QString("/messages.getConversations?");
    requestString += QString("extended=1&");
    requestString += QString("access_token=") + networkSettings->getToken() + '&';
    requestString += QString("v=") + networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION);
    
    QJsonObject jsonReply;
    Error       err{"Getting dialogs error!", true};
    
    if (!m_requestExecutor->executeGetRequest(QUrl(requestString, QUrl::TolerantMode), jsonReply))
        return (m_lastError = err);
    
    if (jsonReply.contains("error"))
        return (m_lastError = Error{QString("Sending message error! ") + jsonReply["error"].toString(), true});
    
    if (!jsonReply.contains("response"))   return err;
    if (!jsonReply["response"].isObject()) return err;
    
    jsonReply = jsonReply["response"].toObject();
    
    if (!jsonReply.contains("items") || !jsonReply.contains("profiles") || !jsonReply.contains("groups"))
        return (m_lastError = err);
    
    std::vector<DialogEntity> dialogsBuffer;
    
    if (!m_entityParser->jsonToDialogs(jsonReply, dialogsBuffer))
        return (m_lastError = err);
    
    dialogs = std::move(dialogsBuffer);
    
    return Error{};
}

Error NetworkDialogsFacadeVK::getPeerName(const EntityInterface::EntityId peerId,
                                          QString &peerName)
{
    return Error{};
}

bool NetworkDialogsFacadeVK::initialize()
{
    auto networkSettings = NetworkSettings::getInstance();
    QString requestString{};
    
    requestString += NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork()) + QString("/messages.getLongPollServer?");
    requestString += QString("access_token=") + networkSettings->getToken() + '&';
    requestString += QString("v=") + networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION);
    
    QJsonObject jsonReply;
    
    if (!m_requestExecutor->executeGetRequest(QUrl(requestString, QUrl::TolerantMode), jsonReply))
        return false;
    
    if (jsonReply.contains("error") || !jsonReply.contains("response")) return false;
    
    jsonReply = jsonReply["response"].toObject();
    
    if (!jsonReply.contains("server") || !jsonReply.contains("key")
     || !jsonReply.contains("ts"))
    {
        return false;
    }
    
    if (!jsonReply["server"].isString()
     || !jsonReply["key"].isString()
     || !jsonReply["ts"].toVariant().isValid())
    {
        return false;
    }
    
    bool isConvOK{false};
    auto ts = jsonReply["ts"].toVariant().toULongLong(&isConvOK);
    
    if (!isConvOK) return false;
    
    QString server = jsonReply["server"].toString(),
            key    = jsonReply["key"].toString();
    
    // setting long poll data to NetworkSettings:
    
    networkSettings->setAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_SERVER_PROP_NAME, server);
    networkSettings->setAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_KEY_PROP_NAME,    key);
    networkSettings->setAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_TS_PROP_NAME,     QString::number(ts));
    
    return true;
}
