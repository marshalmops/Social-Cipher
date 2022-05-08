#ifndef MESSAGEJSONPARSERVK_H
#define MESSAGEJSONPARSERVK_H

#include "MessageJsonParserInterface.h"
#include "AttachmentJsonParserVK.h"

#include "MessageEntityBase.h"

#include "RelatedToMessagesProcessingVK.h"

class MessageJsonParserVK : public MessageJsonParserInterface
{
public:
    constexpr static const char* C_FROM_ID_FIELD_NAME         = "from_id";
    constexpr static const char* C_PEER_ID_FIELD_NAME         = "peer_id";
    constexpr static const char* C_TIMESTAMP_FIELD_NAME       = "date";
    constexpr static const char* C_TEXT_FIELD_NAME            = "text";
    constexpr static const char* C_MESSAGE_ID_FIELD_NAME      = "id";
    constexpr static const char* C_ATTACHMENTS_FIELD_NAME     = "attachments";
    
    constexpr static const uint8_t C_NEW_MESSAGE_EVENT_CODE = 4;
     
    enum EventFields : uint8_t {
        EF_EVENT_ID = 0,
        EF_MESSAGE_ID,
        EF_MESSAGE_FLAGS_SUM,
        EF_PEER_ID,
        EF_TIMESTAMP,
        EF_MESSAGE_TEXT,
        EF_MESSAGE_TITLE,
        EF_MESSAGE_ATTACHMENTS,
        EF_COUNT
    };
    
    enum MessageFlags : uint32_t {
        MF_NOT_READ = 0b0000'0000'0000'0000'0000'0000'0000'0001,
        MF_LOCAL    = 0b0000'0000'0000'0000'0000'0000'0000'0010
    };
    
    MessageJsonParserVK(const std::shared_ptr<AttachmentJsonParserInterface> &attachmentsParser);
    
    virtual bool jsonToMessages(const QJsonValue &json,
                                const RelatedToMessagesProcessingVK::EntityProcessingFlagType messagesProcessingFlag,
                                std::vector<std::shared_ptr<MessageEntityBase>> &messages) const override;
private:
    bool jsonEventsToMessages(const QJsonValue &json,
                              std::vector<std::shared_ptr<MessageEntityBase>> &messages) const;
    bool jsonStandardToMessages(const QJsonValue &json,
                                std::vector<std::shared_ptr<MessageEntityBase>> &messages) const;
};

#endif // MESSAGEJSONPARSERVK_H
