#ifndef INCOMINGMESSAGESPROCESSORBASE_H
#define INCOMINGMESSAGESPROCESSORBASE_H

#include "MessageFilterInterface.h"
#include "MessageJsonParserInterface.h"
#include "AttachmentJsonParserInterface.h"

#include "RelatedToMessagesProcessingBase.h"

class IncomingMessagesProcessorBase
{
public:
    IncomingMessagesProcessorBase(std::unique_ptr<MessageFilterInterface>     &messagesFilter,
                                  std::unique_ptr<MessageJsonParserInterface> &m_messagesParser);
    virtual ~IncomingMessagesProcessorBase() = default;
    
    bool processMessages(const QJsonValue &jsonMessages,
                         const RelatedToMessagesProcessingBase::EntityProcessingFlagType messagesProcessingFlag,
                         std::vector<std::shared_ptr<MessageEntityBase>> &messages);
    
protected:
    std::unique_ptr<MessageFilterInterface> m_messagesFilter;
    
    std::unique_ptr<MessageJsonParserInterface> m_messagesParser;
};

#endif // INCOMINGMESSAGESPROCESSORBASE_H
