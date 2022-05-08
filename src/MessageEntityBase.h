#ifndef MESSAGEENTITYBASE_H
#define MESSAGEENTITYBASE_H

#include "EntityInterface.h"
#include "AttachmentEntityBase.h"

class MessageEntityBase
        : public EntityInterface
{
public:
    using AttachmentsList = std::vector<std::shared_ptr<AttachmentEntityBase>>;
    
    MessageEntityBase();
    MessageEntityBase(const QString &text,
                      const AttachmentsList &attachments = {},
                      const bool isEncrypted = false,
                      const EntityId fromId = 0,
                      const EntityId peerId = 0,
                      const EntityId messageId = 0,
                      const QDateTime &time = QDateTime());
    
    const QString    getText()      const;
    const QDateTime& getTime()      const;
          EntityId   getFromId()    const;
          EntityId   getPeerId()    const;
          EntityId   getMessageId() const;
          bool       isLocal()      const;
          bool       isEncrypted()  const;
          
    const AttachmentsList& getAttachments() const;
    
    bool setText         (const QString &text);
    bool setAttachments  (AttachmentsList &attachments);
    bool appendAttachment(std::shared_ptr<AttachmentEntityBase> &attachment);
    
    void setEncryptedFlag();
    
    //bool swapAttachments(MessageEntityBase &message);
    
    virtual bool isValid() const override;
    
protected:
    EntityId m_messageId;
    
    QString   m_text;
    QDateTime m_time;
    
    EntityId m_fromId;
    EntityId m_peerId;
    
    bool m_isEncrypted;
    
    AttachmentsList m_attachments;
};

#endif // MESSAGEENTITYBASE_H
