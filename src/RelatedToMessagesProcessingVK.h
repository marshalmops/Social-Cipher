#ifndef RELATEDTOMESSAGESPROCESSINGVK_H
#define RELATEDTOMESSAGESPROCESSINGVK_H

#include "RelatedToMessagesProcessingBase.h"

class RelatedToMessagesProcessingVK
        : public RelatedToMessagesProcessingBase
{
public:
    enum MessageProcessingFlag : RelatedToMessagesProcessingBase::EntityProcessingFlagType {
        MPF_IS_EVENT = 0b0000'0000'0000'0001,
        MPF_IS_INIT  = 0b0000'0000'0000'0010
    };
    
    RelatedToMessagesProcessingVK() = delete;
};

#endif // RELATEDTOMESSAGESPROCESSINGVK_H
