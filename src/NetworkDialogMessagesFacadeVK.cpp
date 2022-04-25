#include "NetworkDialogMessagesFacadeVK.h"

NetworkDialogMessagesFacadeVK::NetworkDialogMessagesFacadeVK(const std::shared_ptr<EntityJsonParserVK> &parser,
                                                             const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : NetworkDialogMessagesFacadeInterface{parser, executor},
      m_lastTs{0}
{
    
}

Error NetworkDialogMessagesFacadeVK::sendMessage(const MessageEntity &message,
                                                 const EntityInterface::EntityId peerId) const
{
    auto networkSettings = NetworkSettings::getInstance();
    QString requestString{};
    
    requestString += NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork()) + QString("/messages.send"); /*QString("/messages.send?");
    requestString += QString("peer_id=") + QString::number(peerId) + '&';
    requestString += QString("message=") + message.getText() + '&';
    requestString += QString("random_id=") + QString::number(0) + '&';
    requestString += QString("access_token=") + networkSettings->getToken() + '&';
    requestString += QString("v=") + networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION);*/
    
    QUrlQuery payload;
    
    payload.addQueryItem("peer_id", QString::number(peerId));
    payload.addQueryItem("message", QString::fromUtf8(QUrl::toPercentEncoding(message.getText())));
    payload.addQueryItem("random_id", 0);
    payload.addQueryItem("access_token", QString::fromUtf8(networkSettings->getToken()));
    payload.addQueryItem("v", networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION));
    
    //QJsonObject payload;
    
//    payload["peer_id"]      = QString::number(peerId);
//    payload["message"]      = message.getText();
//    payload["random_id"]    = 0;
//    payload["access_token"] = QString::fromUtf8(networkSettings->getToken());
//    payload["v"]            = networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION);
    
    QJsonObject response;
    
    if (!m_requestExecutor->executePostRequest(QUrl(requestString, QUrl::TolerantMode), payload.toString(QUrl::FullyEncoded).toUtf8(), response))//executeGetRequest(QUrl(requestString, QUrl::TolerantMode), response))
        return (m_lastError = Error{"Sending message error!", true});
    
    if (response.contains("error"))
        return (m_lastError = Error{QString("Sending message error! ") + response["error"].toString(), true});

    return Error{};
}

Error NetworkDialogMessagesFacadeVK::tryGetMessages(std::vector<MessageEntity> &messages)
{
    // long poll request:
    
    auto networkSettings = NetworkSettings::getInstance();
    
    if (m_lastTs == 0) {
        bool isConvOK{false};
        
        m_lastTs = networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_TS_PROP_NAME).toULongLong(&isConvOK);
        
        if (!isConvOK) return (m_lastError = Error{"Long poll ts param value isn't correct!", true});
    }
    
    QString requestString{};
    
    requestString += QString("https://") + networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_SERVER_PROP_NAME) + '?';
    requestString += QString("act=a_check&key=") + networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_KEY_PROP_NAME) + '&';
    requestString += QString("ts=") + QString::number(m_lastTs) + '&';
    requestString += QString("wait=") + networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_WAIT_PROP_NAME) + '&';
    requestString += QString("mode=") + networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_MODE_PROP_NAME) + '&';
    requestString += QString("version=") + networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_VERSION_PROP_NAME);
    
    QJsonObject jsonReply;
    
    auto timeout = networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_LONGPOLL_WAIT_PROP_NAME).toUInt() * 1000;
    
    if (!m_requestExecutor->executeGetRequest(QUrl(requestString, QUrl::TolerantMode), jsonReply, timeout))
        return Error{"Getting message error!", true};
    
    if (jsonReply.empty()) return Error{};  // timeout
    
    if (jsonReply.contains("failed") || !(jsonReply.contains(NetworkFacadeVK::C_LONGPOLL_TS_PROP_NAME)
     && jsonReply.contains("updates")))
    {
        return (m_lastError = Error{"Long poll messages getting error!", true});
    }
    
    auto rawTsValue = jsonReply[NetworkFacadeVK::C_LONGPOLL_TS_PROP_NAME].toVariant().toULongLong();
    
    if (rawTsValue == 0 || m_lastTs > rawTsValue) {
        return (m_lastError = Error{"Long poll messages getting error!", true});
    }
    
    m_lastTs = rawTsValue;
    
    std::vector<MessageEntity> messagesBuffer;
    
    if (!m_entityParser->jsonToMessages(jsonReply["updates"], EntityJsonParserVK::MessageParsingFlag::MPF_IS_EVENT, messagesBuffer))
        return (m_lastError = Error{"Incoming messages parsing error!", true});
    
    // events has reversed order:
    
    //std::reverse(messagesBuffer.begin(), messagesBuffer.end());
    
    messages = std::move(messagesBuffer);
    
    return Error{};
}

Error NetworkDialogMessagesFacadeVK::getDialogMessages(const EntityInterface::EntityId peerId,
                                                       std::vector<MessageEntity> &messages)
{
    // traditional request
    
    auto networkSettings = NetworkSettings::getInstance();
    QString requestString{};
    
    requestString += NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork()) + QString("/messages.getHistory?");
    requestString += QString("peer_id=") + QString::number(peerId) + '&';
    requestString += QString("access_token=") + networkSettings->getToken() + '&';
    requestString += QString("v=") + networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION);
    
    QJsonObject jsonReply;
    Error       err{"Getting messages error!", true};
    
    if (!m_requestExecutor->executeGetRequest(QUrl(requestString, QUrl::TolerantMode), jsonReply))
        return (m_lastError = err);
    
    if (jsonReply.contains("error"))
        return (m_lastError = Error{QString("Getting messages error! ") + jsonReply["error"].toObject()["error_msg"].toString(), true});
    
    if (!jsonReply.contains("response"))   return err;
    if (!jsonReply["response"].isObject()) return err;
    
    jsonReply = jsonReply["response"].toObject();
    
    if (!jsonReply.contains("items")) return err;
    
    std::vector<MessageEntity> messagesBuffer;
    
    if (!m_entityParser->jsonToMessages(jsonReply["items"], 0, messagesBuffer))
        return (m_lastError = err);
    
    std::reverse(messagesBuffer.begin(), messagesBuffer.end());
    
    messages = std::move(messagesBuffer);
    
    return Error{};
}

bool NetworkDialogMessagesFacadeVK::initialize()
{
    return true;
}
