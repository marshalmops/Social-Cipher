#ifndef ATTACHMENTENTITYVKUSINGIDSTRING_H
#define ATTACHMENTENTITYVKUSINGIDSTRING_H

#include "AttachmentEntityVK.h"

class AttachmentEntityVKUsingIdString : public AttachmentEntityVK
{
public:
    AttachmentEntityVKUsingIdString(const EntityId attachmentId,
                                    const EntityId peerId,
                                    const QString &type);
    
    virtual bool isValid() const override;
    virtual RelatedToAttachmentsProcessingVK::AttachmentOriginFlag getProcessingFlag() const override;
    
    QString getType    () const;
    QString getIdString() const;
    
protected:
    QString m_type;
};

#endif // ATTACHMENTENTITYVKUSINGIDSTRING_H
