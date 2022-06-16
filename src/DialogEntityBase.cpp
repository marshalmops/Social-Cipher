#include "DialogEntityBase.h"

DialogEntityBase::DialogEntityBase()
    : m_peerId    {0},
      m_peerName  {},
      m_localPrivateKey{},
      m_localPublicKey {},
      m_remotePublicKey{},
      m_isInitChecked  {false}
{
    
}

DialogEntityBase::DialogEntityBase(const EntityId peerId,
                           const QString peerName,
                           const CipherKey &localPrivateKey,
                           const CipherKey &localPublicKey,
                           const CipherKey &remotePublicKey)
    : m_peerId         {peerId},
      m_peerName       {peerName},
      m_localPrivateKey{localPrivateKey},
      m_localPublicKey {localPublicKey},
      m_remotePublicKey{remotePublicKey},
      m_isInitChecked  {false}
{
}

EntityInterface::EntityId DialogEntityBase::getPeerId() const
{
    return m_peerId;
}

const QString &DialogEntityBase::getPeerName() const
{
    return m_peerName;
}

const DialogEntityBase::MessageListElement& DialogEntityBase::getLastMessage() const
{
    if (m_bufferedMessages.empty()) {
        if (!m_messages.empty()) return m_messages.back();
        else                     return std::unique_ptr<MessageEntityBase>{};
    }
    
    return m_bufferedMessages.back();
}

const DialogEntityBase::MessagesList& DialogEntityBase::getMessages() const
{
    return m_messages;
}

bool DialogEntityBase::isEncrypted() const
{
    if (m_localPrivateKey.getBytes().isEmpty()
     || m_localPublicKey.getBytes().isEmpty()
     || m_remotePublicKey.getBytes().isEmpty())
    {
        return false;
    }
    
    return true;
}

CipherKey DialogEntityBase::getLocalPrivateKey() const
{
    return m_localPrivateKey;
}

CipherKey DialogEntityBase::getLocalPuplicKey() const
{
    return m_localPublicKey;
}

CipherKey DialogEntityBase::getRemotePublicKey() const
{
    return m_remotePublicKey;
}

CipherKey DialogEntityBase::getContentKey() const
{
    return m_contentKey;
}

void DialogEntityBase::resetKeys()
{
    m_localPrivateKey.resetBytes();
    m_localPublicKey.resetBytes();
    m_remotePublicKey.resetBytes();
    m_contentKey.resetBytes();
}

DialogEntityBase::MessageListElement DialogEntityBase::takeBufferedMessage()
{
    if (m_bufferedMessages.empty()) return DialogEntityBase::MessageListElement{nullptr};
    
    DialogEntityBase::MessageListElement message{m_bufferedMessages.front()};
    
    m_bufferedMessages.pop();
    
    return message;
}

bool DialogEntityBase::appendBufferedMessage(const DialogEntityBase::MessageListElement &message)
{
    if (!message->isValid()) return false;
    
    m_bufferedMessages.push(message);
    
    return true;
}

bool DialogEntityBase::appendMessage(const DialogEntityBase::MessageListElement &message)
{
    if (!message->isValid()) return false;
    
    m_messages.push_back(message);
    
    return true;
}

bool DialogEntityBase::setPeerName(const QString &peerName)
{
    if (peerName.isEmpty()) return false;
    
    m_peerName = peerName;
    
    return true;
}

bool DialogEntityBase::setLocalKeys(const CipherKey &localPublicKey,
                                const CipherKey &localPrivateKey)
{
    if (localPublicKey.getBytes().isEmpty()
     || localPrivateKey.getBytes().isEmpty())
    {
        return false;
    }
    
    m_localPrivateKey = localPrivateKey;
    m_localPublicKey  = localPublicKey;
    
    return true;
}

bool DialogEntityBase::setRemoteKey(const CipherKey &remotePublicKey)
{
    if (remotePublicKey.getBytes().isEmpty()) return false;
    
    m_remotePublicKey = remotePublicKey;
    
    return true;
}

bool DialogEntityBase::setContentKey(const CipherKey &contentKey)
{
    if (contentKey.getBytes().isEmpty()) return false;
    
    m_contentKey = contentKey;
    
    return true;
}

bool DialogEntityBase::isInitChecked()
{
    return m_isInitChecked;
}

void DialogEntityBase::setIsInitChecked()
{
    m_isInitChecked = true;
}

bool DialogEntityBase::isValid() const
{
    if (m_peerId == 0) return false;
    
    return true;
}
