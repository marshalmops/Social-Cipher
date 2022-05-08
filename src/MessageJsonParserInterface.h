#ifndef MESSAGEJSONPARSERINTERFACE_H
#define MESSAGEJSONPARSERINTERFACE_H

#include "EntityJsonParserInterface.h"
#include "AttachmentJsonParserInterface.h"

class MessageJsonParserInterface : public EntityJsonParserInterface
{
public:
    MessageJsonParserInterface(const std::shared_ptr<AttachmentJsonParserInterface> &attachmentsParser);
    
    virtual bool jsonToMessages(const QJsonValue &json,
                                const RelatedToMessagesProcessingBase::EntityProcessingFlagType messagesProcessingFlag,
                                std::vector<std::shared_ptr<MessageEntityBase>> &messages) const = 0;

protected:
    std::shared_ptr<AttachmentJsonParserInterface> m_attachmentsParser;

};

#endif // MESSAGEJSONPARSERINTERFACE_H
