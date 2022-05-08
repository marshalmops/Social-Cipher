#include "NetworkDialogMessagesFacadeVK.h"

NetworkDialogMessagesFacadeVK::NetworkDialogMessagesFacadeVK(std::unique_ptr<IncomingMessagesProcessorBase> &messagesProcessor,
                                                             const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : NetworkDialogMessagesFacadeInterface{messagesProcessor, executor},
      m_lastTs{0}
{
    
}

Error NetworkDialogMessagesFacadeVK::sendMessage(const std::shared_ptr<MessageEntityBase> &message,
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
    payload.addQueryItem("message", QString::fromUtf8(QUrl::toPercentEncoding(message->getText())));
    
    QString attachmentsString{};
    
    for (auto i = 0; i < message->getAttachments().size(); ++i) {
        AttachmentEntityVKUsingIdString *attachment{dynamic_cast<AttachmentEntityVKUsingIdString*>(message->getAttachments().at(i).get())};
        QString attachmentTypeIdString{attachment->getType() + attachment->getIdString()};
        
        attachmentsString += (attachmentTypeIdString + (i + 1 == message->getAttachments().size() ? ' ' : ','));
    }
    
    if (!attachmentsString.isEmpty())
        payload.addQueryItem("attachment", attachmentsString);
    
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

std::shared_ptr<MessageEntityBase> NetworkDialogMessagesFacadeVK::generateCommandMessage(const CommandCode command, 
                                                                                         const QString &content) const
{
    QString messageText = QString(C_COMMAND_START) + getStringByCommandCode(command) + content;
    std::shared_ptr<MessageEntityBase> message{std::make_shared<MessageEntityBase>(messageText)};
    
    return message;
}

Error NetworkDialogMessagesFacadeVK::getMessageById(const EntityInterface::EntityId peerId,
                                                    const EntityInterface::EntityId messageId,
                                                    std::shared_ptr<MessageEntityBase> &gottenMessage)
{
    Error err{"Message getting error!", true};
    
    auto networkSettings = NetworkSettings::getInstance();
    QString requestString{};
    
    requestString += NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork()) + QString("/messages.getHistory?");
    requestString += QString("peer_id=") + QString::number(peerId) + '&';
    requestString += QString("start_message_id=") + QString::number(messageId) + '&';
    requestString += QString("count=") + QString::number(1) + '&';
    requestString += QString("access_token=") + networkSettings->getToken() + '&';
    requestString += QString("v=") + networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION);
    
    QJsonObject jsonReply;
    
    if (!m_requestExecutor->executeGetRequest(QUrl(requestString, QUrl::TolerantMode), jsonReply))
        return (m_lastError = err);
    
    if (jsonReply.contains("error"))
        return (m_lastError = err);
    
    if (!jsonReply.contains("response"))   return err;
    if (!jsonReply["response"].isObject()) return err;
    
    jsonReply = jsonReply["response"].toObject();
    
    if (!jsonReply.contains("items")) return err;
    
    std::vector<std::shared_ptr<MessageEntityBase>> messagesBuffer;
    
    if (!m_messagesProcessor->processMessages(jsonReply["items"], 0, messagesBuffer))
        return (m_lastError = err);
    if (messagesBuffer.size() != 1) return (m_lastError = err);
    
    gottenMessage = std::move(messagesBuffer.front());
    
    return Error{};
}

Error NetworkDialogMessagesFacadeVK::tryGetMessages(std::vector<std::shared_ptr<MessageEntityBase>> &messages)
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
    
    bool isFailedWithTs{false};
    
    if (jsonReply.contains("failed")) {
        if (jsonReply["failed"].toInt(0) != 1) 
            return (m_lastError = Error{"Long poll request is failed!", true});
        
        isFailedWithTs = true;
        
    } else {
        if (!jsonReply.contains("updates"))
            return (m_lastError = Error{"Long poll messages getting error!", true});
    }
    
    if (!jsonReply.contains(NetworkFacadeVK::C_LONGPOLL_TS_PROP_NAME))
        return (m_lastError = Error{"Long poll messages getting error!", true});
    
    auto rawTsValue = jsonReply[NetworkFacadeVK::C_LONGPOLL_TS_PROP_NAME].toVariant().toULongLong();
    
    if (rawTsValue == 0/* || m_lastTs > rawTsValue*/) {
        return (m_lastError = Error{"Long poll messages getting error!", true});
    }
    
    m_lastTs = rawTsValue;
    
    if (!isFailedWithTs) {
        std::vector<std::shared_ptr<MessageEntityBase>> messagesBuffer;
        
        if (!m_messagesProcessor->processMessages(jsonReply["updates"], RelatedToMessagesProcessingVK::MessageProcessingFlag::MPF_IS_EVENT, messagesBuffer))
            return (m_lastError = Error{"Incoming messages processing error!", true});
        
        // converting attachments using id string to link-related ones:
        
        for (auto iter = messagesBuffer.begin(); iter != messagesBuffer.end(); ++iter) {
            auto &curMessage {*iter};
            auto &curAttachments{curMessage->getAttachments()};
            
            if (curAttachments.size() <= 0) continue;
            
            // getting message using standard way (with attachments using URLs)...
            
            std::shared_ptr<MessageEntityBase> gottenMessage;
            
            Error gettingMessageError{};
            
            if ((gettingMessageError = getMessageById(curMessage->getPeerId(), curMessage->getMessageId(), gottenMessage)).isValid())
                return (m_lastError = gettingMessageError);
            
            curMessage.swap(gottenMessage);
        }
        
//        if (!m_messagesParser->jsonToMessages(jsonReply["updates"], MessageJsonParserVK::MessageParsingFlag::MPF_IS_EVENT, messagesBuffer))
//            return (m_lastError = Error{"Incoming messages parsing error!", true});
        
//        std::vector<std::unique_ptr<MessageEntityBase>> filteredMessagesBuffer;
        
//        if (!m_messagesFilter->filterMessagesByAttachments(messagesBuffer, filteredMessagesBuffer))
//            return (m_lastError = Error{"Incoming messages filtering error!", true});
        
        messages = std::move(messagesBuffer);
    }
    
    return Error{};
}

Error NetworkDialogMessagesFacadeVK::getDialogMessages(const EntityInterface::EntityId peerId,
                                                       std::vector<std::shared_ptr<MessageEntityBase>> &messages,
                                                       const uint8_t messagesCount)
{
    // traditional request
    
    auto networkSettings = NetworkSettings::getInstance();
    QString requestString{};
    
    requestString += NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork()) + QString("/messages.getHistory?");
    requestString += QString("peer_id=") + QString::number(peerId) + '&';
    requestString += QString("count=") + QString::number(messagesCount) + '&';
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
    
    std::vector<std::shared_ptr<MessageEntityBase>> messagesBuffer;
    
    if (!m_messagesProcessor->processMessages(jsonReply["items"], 0, messagesBuffer))
        return (m_lastError = err);
    
//    if (!m_messagesParser->jsonToMessages(jsonReply["items"], 0, messagesBuffer))
//        return (m_lastError = err);
    
//    std::vector<std::unique_ptr<MessageEntityBase>> filteredMessagesBuffer;
    
//    if (!m_messagesFilter->filterMessagesByAttachments(messagesBuffer, filteredMessagesBuffer))
//        return (m_lastError = Error{"Incoming messages filtering error!", true});
    
    std::reverse(messagesBuffer.begin(), messagesBuffer.end());
    
    messages = std::move(messagesBuffer);
    
    return Error{};
}

bool NetworkDialogMessagesFacadeVK::initialize()
{
    return true;
}
