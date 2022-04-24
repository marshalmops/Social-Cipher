#include "EntityJsonParserVK.h"

EntityJsonParserVK::EntityJsonParserVK()
{
    
}

bool EntityJsonParserVK::jsonToMessages(const QJsonValue &json, 
                                        const ParsingFlag flags,
                                        std::vector<MessageEntity> &messages) const
{
    if (json.isNull())   return false;
    if (!json.isArray()) return false;
    
    QJsonArray jsonRawMessagesArray = json.toArray();
    
    if (flags & MessageParsingFlag::MPF_IS_EVENT) {
        // event parsing...
        
        if (!jsonEventsToMessages(jsonRawMessagesArray, messages)) return false;
        
    } else {
        // standard request response parsing...
        
        if (!jsonStandardToMessages(jsonRawMessagesArray, messages)) return false;
    }
    
    return true;
}

bool EntityJsonParserVK::jsonToDialogs(const QJsonValue &json,
                                       std::vector<DialogEntity> &dialogs) const
{
    // WHOLE DIALOGS PARSING (INCL. CONTAINED MESSAGES):
    
    if (json.isNull())    return false;
    if (!json.isObject()) return false;
    
    QJsonObject jsonDialogs = json.toObject();
    
    if (!jsonDialogs["items"].isArray() || !jsonDialogs["profiles"].isArray() || !jsonDialogs["groups"].isArray())
        return false;
    
    QJsonArray conversationsJsonArray = jsonDialogs["items"].toArray();
    QJsonArray profilesJsonArray      = jsonDialogs["profiles"].toArray();
    QJsonArray groupsJsonArray        = jsonDialogs["groups"].toArray();
    
    // conversations scanning with profile or group name searching:
    
    foreach (const auto &dialog, conversationsJsonArray) {
        if (!dialog.isObject()) return false;
        
        auto curDialog = dialog.toObject();
        
        if (curDialog.isEmpty())                   continue;
        if (!curDialog.contains("conversation"))   continue;
        if (!curDialog["conversation"].isObject()) continue;
        
        curDialog = curDialog["conversation"].toObject();
        
        if (!curDialog.contains("peer"))                  continue;
        if (!curDialog["peer"].isObject())                continue;
        if (!curDialog["peer"].toObject().contains("id")) continue;
        
        curDialog = curDialog["peer"].toObject();
        
        DialogEntity::EntityId peerId = curDialog["id"].toVariant().toLongLong();
        QString peerName;
        
//        if (peerId == networkSettings->getLocalPeerId()) continue;
        
        if (!findPeerNameInProfilesGroups(peerId, profilesJsonArray, groupsJsonArray, peerName)) {
            // multiple-user conversations case...
            
            continue;
        }
        
        DialogEntity newDialog{peerId, peerName};
        
//        if (!newDialog.isValid()) return false;
        
        dialogs.push_back(std::move(newDialog));
    }
    
    return true;
}

bool EntityJsonParserVK::jsonEventsToMessages(const QJsonValue &json,
                                              std::vector<MessageEntity> &messages) const
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
        
        if (!isConvOK)              return false;
        //if (messageFlag & MF_LOCAL) continue;
            
        QString                 messageText     {curEvent[EF_MESSAGE_TEXT].toString()};
        QDateTime               messageTimestamp{QDateTime::fromSecsSinceEpoch(curEvent[EF_TIMESTAMP].toInt())};
        MessageEntity::EntityId messagePeer     {curEvent[EF_PEER_ID].toInt()};
        MessageEntity::EntityId messageId       {curEvent[EF_MESSAGE_ID].toInt()};
        
        MessageEntity newMessage{messageText, false, (messageFlag & MF_LOCAL ? 0 : messagePeer), messagePeer, messageId, messageTimestamp};
        
        if (!newMessage.isValid()) return false;
        
        messages.push_back(std::move(newMessage));
    }
    
    return true;
}

bool EntityJsonParserVK::jsonStandardToMessages(const QJsonValue &json,
                                                std::vector<MessageEntity> &messages) const
{
    if (!json.isArray()) return false;
    
    foreach (const auto &jsonVal, json.toArray()) {
        if (!jsonVal.isObject()) return false;
        
        const auto curMessage = jsonVal.toObject();
        
        if (curMessage.isEmpty()) continue;
        
        // message processing:
        
        if (!curMessage.contains(C_PEER_ID_FIELD_NAME)
         || !curMessage.contains(C_TEXT_FIELD_NAME)
         || !curMessage.contains(C_TIMESTAMP_FIELD_NAME))
        {
            continue;
        }
            
        QString                 messageText     {curMessage[C_TEXT_FIELD_NAME].toString()};
        QDateTime               messageTimestamp{QDateTime::fromSecsSinceEpoch(curMessage[C_TIMESTAMP_FIELD_NAME].toInt())};
        MessageEntity::EntityId messagePeer     {curMessage[C_PEER_ID_FIELD_NAME].toInt()};
        MessageEntity::EntityId messageFromId   {curMessage[C_FROM_ID_FIELD_NAME].toInt()};
        MessageEntity::EntityId messageId       {curMessage[C_MESSAGE_ID_FIELD_NAME].toInt()};
        
        MessageEntity newMessage{messageText, false, messageFromId, messagePeer, messageId, messageTimestamp};
        
        if (!newMessage.isValid()) return false;
        
        messages.push_back(std::move(newMessage));
    }
    
    return true;
}

bool EntityJsonParserVK::findPeerNameInProfilesGroups(const DialogEntity::EntityId peerId,
                                                      const QJsonArray &profilesJsonArray,
                                                      const QJsonArray &groupsJsonArray,
                                                      QString &peerName) const
{
    foreach (const auto &profile, profilesJsonArray) {
        QString peerNameBuffer;
        
        if (!checkPeerNameInJsonObject({"first_name", "last_name"}, peerId, profile, peerNameBuffer))
            return false;
        
        if (!peerNameBuffer.isEmpty()) {
            peerName = peerNameBuffer;
            
            return true;
        }
    }
    
    foreach (const auto &group, groupsJsonArray) {
        QString peerNameBuffer;
        
        if (!checkPeerNameInJsonObject({"name"}, peerId, group, peerNameBuffer))
            return false;
        
        if (!peerNameBuffer.isEmpty()) {
            peerName = peerNameBuffer;
            
            return true;
        }
    }
    
    return false;
}

bool EntityJsonParserVK::checkPeerNameInJsonObject(const QStringList &peerNameParamsNames,
                                                   const EntityInterface::EntityId peerId,
                                                   const QJsonValue &jsonVal,
                                                   QString &peerName) const
{
    if (!jsonVal.isObject()) return false;
    
    QJsonObject curJsonObj = jsonVal.toObject();
    
    foreach (const auto &curParamName, peerNameParamsNames) {
        if (!curJsonObj.contains(curParamName)) return false;
    }
    
    if (!curJsonObj.contains("id"))              return false;
    if (!curJsonObj["id"].toVariant().isValid()) return false;
    
    bool isConvOK{false};
    auto curPeerId = curJsonObj["id"].toVariant().toLongLong(&isConvOK);
    
    if (!isConvOK) return false;
    
    if (curPeerId == peerId) {
        for (auto i = 0; i < peerNameParamsNames.length(); ++i) {
            if (!curJsonObj[peerNameParamsNames[i]].isString()) return false;
            
            peerName += (curJsonObj[peerNameParamsNames[i]].toString() + (i == peerNameParamsNames.length() - 1 ? "" : " "));
        }
    }
    
    return true;
}
