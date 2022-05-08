#ifndef DIALOGENTITYBASE_H
#define DIALOGENTITYBASE_H

#include <queue>

#include "EntityInterface.h"
#include "MessageEntityBase.h"

#include "CipherKey.h"

class DialogEntityBase : public EntityInterface
{
public:
    using MessageListElement = std::shared_ptr<MessageEntityBase>;
    using MessagesList       = std::vector<MessageListElement>;
    
    DialogEntityBase();
    DialogEntityBase(const EntityId peerId,
                 const QString peerName = QString(),
                 const CipherKey &localPrivateKey = CipherKey(),
                 const CipherKey &localPublicKey = CipherKey(),
                 const CipherKey &remotePublicKey = CipherKey());
    
    EntityId                  getPeerId     () const;
    const QString&            getPeerName   () const;
    const MessageListElement& getLastMessage() const;
    const MessagesList&       getMessages   () const;
    
    bool isEncrypted() const;
    
    CipherKey getLocalPrivateKey() const;
    CipherKey getLocalPuplicKey () const;
    CipherKey getRemotePublicKey() const;
    
    void resetKeys();
    
    MessageListElement takeBufferedMessage();
    
    bool appendBufferedMessage(const MessageListElement &message);
    bool appendMessage        (const MessageListElement &message);

    bool setPeerName (const QString &peerName);
    bool setLocalKeys(const CipherKey &localPublicKey,
                      const CipherKey &localPrivateKey);
    bool setRemoteKey(const CipherKey &remotePublicKey);
    
    bool isInitChecked();
    void setIsInitChecked();
    
    virtual bool isValid() const override;
    
protected:
    EntityId      m_peerId;
    QString       m_peerName;
    
    MessagesList                   m_messages;
    std::queue<MessageListElement> m_bufferedMessages;
    
    CipherKey m_localPrivateKey;
    CipherKey m_localPublicKey;
    CipherKey m_remotePublicKey;
    
    bool m_isInitChecked;
};

#endif // DIALOGENTITYBASE_H
