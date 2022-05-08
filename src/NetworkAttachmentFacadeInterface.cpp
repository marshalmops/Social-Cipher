#include "NetworkAttachmentFacadeInterface.h"

NetworkAttachmentFacadeInterface::NetworkAttachmentFacadeInterface(const std::shared_ptr<AttachmentJsonParserInterface> &attachmentParser,
                                                                   const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : NetworkFacadeInterface{executor},
      m_attachmentParser    {attachmentParser}
{
    
}

bool NetworkAttachmentFacadeInterface::initialize()
{
    return true;
}
