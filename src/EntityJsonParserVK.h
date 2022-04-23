#ifndef ENTITYJSONPARSERVK_H
#define ENTITYJSONPARSERVK_H

#include "EntityJsonParserInterface.h"

class EntityJsonParserVK : public EntityJsonParserInterface
{
public:
    constexpr static const char* C_FROM_ID_FIELD_NAME    = "from_id";
    constexpr static const char* C_PEER_ID_FIELD_NAME    = "peer_id";
    constexpr static const char* C_TIMESTAMP_FIELD_NAME  = "date";
    constexpr static const char* C_TEXT_FIELD_NAME       = "text";
    constexpr static const char* C_MESSAGE_ID_FIELD_NAME = "id";
    
    constexpr static const uint8_t C_NEW_MESSAGE_EVENT_CODE = 4;
     
    enum EventFields : uint8_t {
        EF_EVENT_ID = 0,
        EF_MESSAGE_ID,
        EF_MESSAGE_FLAGS_SUM,
        EF_PEER_ID,
        EF_TIMESTAMP,
        EF_MESSAGE_TEXT,
        EF_COUNT
    };
    
    enum MessageFlags : uint32_t {
        MF_NOT_READ = 0b0000'0000'0000'0000'0000'0000'0000'0001,
        MF_LOCAL    = 0b0000'0000'0000'0000'0000'0000'0000'0010
    };
    
    enum MessageParsingFlag : ParsingFlag {
        MPF_IS_EVENT = 0b0000'0000'0000'0001,
        MPF_IS_INIT  = 0b0000'0000'0000'0010
    };
    
    EntityJsonParserVK();
    
    virtual bool jsonToMessages(const QJsonValue &json, 
                                const ParsingFlag flags,
                                std::vector<MessageEntity> &messages) const override;
    virtual bool jsonToDialogs (const QJsonValue &json,
                                std::vector<DialogEntity> &dialogs) const override;
    
protected:
    bool jsonEventsToMessages(const QJsonValue &json,
                              std::vector<MessageEntity> &messages) const;
    bool jsonStandardToMessages(const QJsonValue &json,
                                std::vector<MessageEntity> &messages) const;
    
    bool findPeerNameInProfilesGroups(const EntityInterface::EntityId peerId,
                                      const QJsonArray &profilesJsonArray,
                                      const QJsonArray &groupsJsonArray,
                                      QString &peerName) const;
    
    bool checkPeerNameInJsonObject(const QStringList &peerNameParamsNames,
                                   const EntityInterface::EntityId peerId,
                                   const QJsonValue &jsonVal,
                                   QString &peerName) const;
    
};

#endif // ENTITYJSONPARSERVK_H
