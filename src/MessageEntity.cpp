#include "MessageEntity.h"

MessageEntity::MessageEntity()
{
    
}

MessageEntity::MessageEntity(const QString &text, 
                             const bool isEncrypted, 
                             const EntityId fromId,
                             const EntityId peerId,
                             const EntityId messageId,
                             const QDateTime &time)
    : m_text        {text},
      m_isEncrypted {isEncrypted},
      m_fromId      {fromId},
      m_peerId      {peerId},
      m_messageId   {messageId},
      m_time        {time}
{
    
}

const QString MessageEntity::getText() const
{
    return (m_text.isEmpty() ? "(not readable)" : m_text);
}

const QDateTime &MessageEntity::getTime() const
{
    return m_time;
}

EntityInterface::EntityId MessageEntity::getFromId() const
{
    return m_fromId;
}

EntityInterface::EntityId MessageEntity::getPeerId() const
{
    return m_peerId;
}

EntityInterface::EntityId MessageEntity::getMessageId() const
{
    return m_messageId;
}

bool MessageEntity::isLocal() const
{
    if (m_fromId == m_peerId) return false;
    
    return true;
}

bool MessageEntity::isEncrypted() const
{
    return m_isEncrypted;
}

bool MessageEntity::isValid() const
{
    if (/*m_text.isEmpty() ||*/ (m_fromId != 0 && (!m_time.isValid() || m_messageId == 0))) return false;
    
    return true;
}
