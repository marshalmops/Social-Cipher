#include "MessageEntityBase.h"

MessageEntityBase::MessageEntityBase()
{
    
}

MessageEntityBase::MessageEntityBase(const QString &text, 
                                     const AttachmentsList &attachments,
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
      m_time        {time},
      m_attachments {attachments}
{
    
}

const QString MessageEntityBase::getText() const
{
    return ((m_text.isEmpty() && m_attachments.empty()) ? QString{"(not readable)"} : m_text);
}

const QDateTime &MessageEntityBase::getTime() const
{
    return m_time;
}

EntityInterface::EntityId MessageEntityBase::getFromId() const
{
    return m_fromId;
}

EntityInterface::EntityId MessageEntityBase::getPeerId() const
{
    return m_peerId;
}

EntityInterface::EntityId MessageEntityBase::getMessageId() const
{
    return m_messageId;
}

bool MessageEntityBase::isLocal() const
{
    if (m_fromId == m_peerId) return false;
    
    return true;
}

bool MessageEntityBase::isEncrypted() const
{
    return m_isEncrypted;
}

const MessageEntityBase::AttachmentsList &MessageEntityBase::getAttachments() const
{
    return m_attachments;
}

bool MessageEntityBase::setText(const QString &text)
{
    if (text.isEmpty() && !m_text.isEmpty()) return false;
    
    m_text = text;
    
    return true;
}

bool MessageEntityBase::setAttachments(AttachmentsList &attachments)
{
    if (attachments.empty()) return false;
    
    m_attachments = std::move(attachments);
    
    return true;
}

bool MessageEntityBase::appendAttachment(std::shared_ptr<AttachmentEntityBase> &attachment)
{
    m_attachments.push_back(attachment);
}

void MessageEntityBase::setEncryptedFlag()
{
    m_isEncrypted = true;
}

//bool MessageEntityBase::swapAttachments(MessageEntityBase &message)
//{
//    if (message.m_attachments.empty()) return false;
    
//    auto localAttachments{std::move(m_attachments)};
    
//    m_attachments         = std::move(message.m_attachments);
//    message.m_attachments = std::move(localAttachments);
    
//    return true;
//}

bool MessageEntityBase::isValid() const
{
    if (/*m_text.isEmpty() ||*/ (m_fromId != 0 && (!m_time.isValid() || m_messageId == 0)) || (m_text.isEmpty() && m_attachments.empty())) return false;
    
    return true;
}
