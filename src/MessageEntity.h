#ifndef MESSAGEENTITY_H
#define MESSAGEENTITY_H

#include "EntityInterface.h"

class MessageEntity : public EntityInterface
{
public:
    MessageEntity();
    MessageEntity(const QString &text,
                  const bool isEncrypted = false,
                  const EntityId fromId = 0,
                  const EntityId peerId = 0,
                  const EntityId messageId = 0,
                  const QDateTime &time = QDateTime());
    
//    virtual bool fromJSONObject(const QJsonObject &obj) override;
//    virtual QJsonObject toJSONObject() const override;
    
    const QString    getText()      const;
    const QDateTime& getTime()      const;
          EntityId   getFromId()    const;
          EntityId   getPeerId()    const;
          EntityId   getMessageId() const;
          bool       isLocal()      const;
          bool       isEncrypted()  const;
    
    virtual bool isValid() const override;
    
private:
    EntityId m_messageId;
    
    QString   m_text;
    QDateTime m_time;
    
    EntityId m_fromId;
    EntityId m_peerId;
    
    bool m_isEncrypted;
};

#endif // MESSAGEENTITY_H
