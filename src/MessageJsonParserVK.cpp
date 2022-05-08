#include "MessageJsonParserVK.h"

MessageJsonParserVK::MessageJsonParserVK(const std::shared_ptr<AttachmentJsonParserInterface> &attachmentsParser)
    : MessageJsonParserInterface{attachmentsParser}
{
    
}

bool MessageJsonParserVK::jsonToMessages(const QJsonValue &json,
                                         const RelatedToEntityProcessingBase::EntityProcessingFlagType messagesProcessingFlag, 
                                         std::vector<std::shared_ptr<MessageEntityBase>> &messages) const
{
    if (json.isNull())   return false;
    if (!json.isArray()) return false;
    
    QJsonArray jsonRawMessagesArray = json.toArray();
    
    if (messagesProcessingFlag & RelatedToMessagesProcessingVK::MessageProcessingFlag::MPF_IS_EVENT) {
        // event parsing...
        
        if (!jsonEventsToMessages(jsonRawMessagesArray, messages)) return false;
        
    } else {
        // standard request response parsing...
        
        if (!jsonStandardToMessages(jsonRawMessagesArray, messages)) return false;
    }
    
    return true;
}

bool MessageJsonParserVK::jsonEventsToMessages(const QJsonValue &json,
                                               std::vector<std::shared_ptr<MessageEntityBase>> &messages) const
{
    if (!json.isArray()) return false;
    
    foreach (const auto &jsonVal, json.toArray()) {
        if (!jsonVal.isArray()) return false;
        
        const auto curEvent = jsonVal.toArray();
        
        if (curEvent.isEmpty()) continue;
        
        // event processing:
        
        if (curEvent[EF_EVENT_ID].toInt() != C_NEW_MESSAGE_EVENT_CODE
         || curEvent.size() < EF_COUNT)
        {
            continue;
        }
        
        bool isConvOK{false};
        uint32_t messageFlag = curEvent[EF_MESSAGE_FLAGS_SUM].toVariant().toUInt(&isConvOK);
        
        if (!isConvOK)                                    return false;
        if (messageFlag & MF_LOCAL)                       continue;
        if (!curEvent[EF_MESSAGE_ATTACHMENTS].isObject()) return false;
            
        QString                     messageText     {curEvent[EF_MESSAGE_TEXT].toString()};
        QDateTime                   messageTimestamp{QDateTime::fromSecsSinceEpoch(curEvent[EF_TIMESTAMP].toInt())};
        MessageEntityBase::EntityId messagePeer     {curEvent[EF_PEER_ID].toInt()};
        MessageEntityBase::EntityId messageId       {curEvent[EF_MESSAGE_ID].toInt()};
        
        QJsonObject messageAttachments{curEvent[EF_MESSAGE_ATTACHMENTS].toObject()};
        std::vector<std::shared_ptr<AttachmentEntityBase>> attachmentsList{};
        
        m_attachmentsParser->jsonToAttachments(messageAttachments, RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_EVENTIONAL, attachmentsList);
        
        MessageEntityBase newMessage{messageText, attachmentsList, false, messagePeer, messagePeer, messageId, messageTimestamp};
        
        if (!newMessage.isValid()) continue; //return false;
        
        messages.push_back(std::make_shared<MessageEntityBase>(std::move(newMessage)));
    }
    
    return true;
}

bool MessageJsonParserVK::jsonStandardToMessages(const QJsonValue &json,
                                                 std::vector<std::shared_ptr<MessageEntityBase>> &messages) const
{
    if (!json.isArray()) return false;
    
    foreach (const auto &jsonVal, json.toArray()) {
        if (!jsonVal.isObject()) return false;
        
        const auto curMessage = jsonVal.toObject();
        
        if (curMessage.isEmpty()) continue;
        
        // message processing:
        
        if (!curMessage.contains(C_PEER_ID_FIELD_NAME)
         || !curMessage.contains(C_TEXT_FIELD_NAME)
         || !curMessage.contains(C_TIMESTAMP_FIELD_NAME)
         || !curMessage.contains(C_ATTACHMENTS_FIELD_NAME))
        {
            continue;
        }
            
        QString                     messageText     {curMessage[C_TEXT_FIELD_NAME].toString()};
        QDateTime                   messageTimestamp{QDateTime::fromSecsSinceEpoch(curMessage[C_TIMESTAMP_FIELD_NAME].toInt())};
        MessageEntityBase::EntityId messagePeer     {curMessage[C_PEER_ID_FIELD_NAME].toInt()};
        MessageEntityBase::EntityId messageFromId   {curMessage[C_FROM_ID_FIELD_NAME].toInt()};
        MessageEntityBase::EntityId messageId       {curMessage[C_MESSAGE_ID_FIELD_NAME].toInt()};
        
        if (!curMessage[C_ATTACHMENTS_FIELD_NAME].isArray())
            return false;
        
        auto attachmentsJsonArray = curMessage[C_ATTACHMENTS_FIELD_NAME].toArray();
        std::vector<std::shared_ptr<AttachmentEntityBase>> attachmentsList{};
        
        m_attachmentsParser->jsonToAttachments(attachmentsJsonArray, RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_STANDARD, attachmentsList);
        
        MessageEntityBase newMessage{messageText, attachmentsList, false, messageFromId, messagePeer, messageId, messageTimestamp};
        
        if (!newMessage.isValid()) continue; //return false;
        
        messages.push_back(std::make_shared<MessageEntityBase>(std::move(newMessage)));
    }
    
    return true;
}
