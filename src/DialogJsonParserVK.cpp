#include "DialogJsonParserVK.h"

DialogJsonParserVK::DialogJsonParserVK()
{
    
}

bool DialogJsonParserVK::jsonToDialogs(const QJsonValue &json,
                                       std::vector<std::unique_ptr<DialogEntityBase>> &dialogs) const
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
        
        DialogEntityBase::EntityId peerId = curDialog["id"].toVariant().toLongLong();
        QString peerName;
        
        if (!findPeerNameInProfilesGroups(peerId, profilesJsonArray, groupsJsonArray, peerName)) {
            // multiple-user conversations case...
            
            continue;
        }
        
        DialogEntityBase newDialog{peerId, peerName};
        
        dialogs.push_back(std::make_unique<DialogEntityBase>(std::move(newDialog)));
    }
    
    return true;
}

bool DialogJsonParserVK::jsonToFullName(const QJsonValue &json, 
                                        QString &fullName)
{
    if (json.isNull())   return false;
    if (!json.isObject()) return false;
    
    QJsonObject jsonObject{json.toObject()};
    
    if (!jsonObject.contains("first_name") || !jsonObject.contains("last_name"))
        return false;
    
    if (!jsonObject["first_name"].isString() || !jsonObject["last_name"].isString())
        return false;
    
    QString fullNameBuffer{jsonObject["first_name"].toString() + " " + jsonObject["last_name"].toString()};

    if (fullNameBuffer.isEmpty()) return false;
    
    fullName = fullNameBuffer;
    
    return true;
}

bool DialogJsonParserVK::findPeerNameInProfilesGroups(const EntityInterface::EntityId peerId,
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

bool DialogJsonParserVK::checkPeerNameInJsonObject(const QStringList &peerNameParamsNames,
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
