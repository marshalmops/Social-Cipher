#include "IncomingMessagesProcessorBase.h"

IncomingMessagesProcessorBase::IncomingMessagesProcessorBase(std::unique_ptr<MessageFilterInterface> &messagesFilter,
                                                             std::unique_ptr<MessageJsonParserInterface> &m_messagesParser)
    : m_messagesFilter{messagesFilter.release()},
      m_messagesParser{m_messagesParser.release()}
{
    
}

bool IncomingMessagesProcessorBase::processMessages(const QJsonValue &jsonMessages,
                                                    const RelatedToMessagesProcessingBase::EntityProcessingFlagType messagesProcessingFlag,
                                                    std::vector<std::shared_ptr<MessageEntityBase>> &messages)
{
    std::vector<std::shared_ptr<MessageEntityBase>> messagesBuffer;
    
    if (!m_messagesParser->jsonToMessages(jsonMessages, messagesProcessingFlag, messagesBuffer))
        return false;
    
    std::vector<std::shared_ptr<MessageEntityBase>> filteredMessagesBuffer;
    
    if (!m_messagesFilter->filterMessages(messagesBuffer, messagesProcessingFlag, filteredMessagesBuffer))
        return false;
    
    messages = std::move(filteredMessagesBuffer);
    
    return true;
}
