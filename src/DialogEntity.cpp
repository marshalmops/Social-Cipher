#include "DialogEntity.h"

DialogEntity::DialogEntity()
    : m_peerId    {0},
      m_peerName  {},
      m_localPrivateKey{},
      m_localPublicKey {},
      m_remotePublicKey{},
      m_isInitChecked  {false}
{
    
}

DialogEntity::DialogEntity(const EntityId peerId,
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

EntityInterface::EntityId DialogEntity::getPeerId() const
{
    return m_peerId;
}

const QString &DialogEntity::getPeerName() const
{
    return m_peerName;
}

const MessageEntity& DialogEntity::getLastMessage() const
{
    if (m_bufferedMessages.empty()) {
        if (!m_messages.empty()) return m_messages.back();
        else                     return MessageEntity{};
    }
    
    return m_bufferedMessages.back();
}

const std::vector<MessageEntity> &DialogEntity::getMessages() const
{
    return m_messages;
}

bool DialogEntity::isEncrypted() const
{
    if (m_localPrivateKey.getBytes().isEmpty()
     || m_localPublicKey.getBytes().isEmpty()
     || m_remotePublicKey.getBytes().isEmpty())
    {
        return false;
    }
    
    return true;
}

CipherKey DialogEntity::getLocalPrivateKey() const
{
    return m_localPrivateKey;
}

CipherKey DialogEntity::getLocalPuplicKey() const
{
    return m_localPublicKey;
}

CipherKey DialogEntity::getRemotePublicKey() const
{
    return m_remotePublicKey;
}

void DialogEntity::resetKeys()
{
    m_localPrivateKey.resetBytes();
    m_localPublicKey.resetBytes();
    m_remotePublicKey.resetBytes();
}

MessageEntity DialogEntity::takeBufferedMessage()
{
    if (m_bufferedMessages.empty()) return MessageEntity{};
    
    //MessageEntity message = m_bufferedMessages.back();
    
    //m_bufferedMessages.pop_back();
    
    MessageEntity message = m_bufferedMessages.front();
    
    m_bufferedMessages.pop();
    
    return message;
}

bool DialogEntity::appendBufferedMessage(const MessageEntity &message)
{
    if (!message.isValid()) return false;
    
    //m_bufferedMessages.push_back(message);
    m_bufferedMessages.push(message);
    
    return true;
}

bool DialogEntity::appendMessage(const MessageEntity &message)
{
    if (!message.isValid()) return false;
    
    m_messages.push_back(message);
    
    return true;
}

bool DialogEntity::setPeerName(const QString &peerName)
{
    if (peerName.isEmpty()) return false;
    
    m_peerName = peerName;
    
    return true;
}

bool DialogEntity::setLocalKeys(const CipherKey &localPublicKey,
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

bool DialogEntity::setRemoteKey(const CipherKey &remotePublicKey)
{
    if (remotePublicKey.getBytes().isEmpty()) return false;
    
    m_remotePublicKey = remotePublicKey;
    
    return true;
}

bool DialogEntity::isInitChecked()
{
    return m_isInitChecked;
}

void DialogEntity::setIsInitChecked()
{
    m_isInitChecked = true;
}

bool DialogEntity::isValid() const
{
    if (m_peerId == 0) return false;
    
    return true;
}
