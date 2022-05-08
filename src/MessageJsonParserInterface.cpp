#include "MessageJsonParserInterface.h"

MessageJsonParserInterface::MessageJsonParserInterface(const std::shared_ptr<AttachmentJsonParserInterface> &attachmentsParser)
    : m_attachmentsParser{attachmentsParser}
{
    
}
