#ifndef DIALOGENTITY_H
#define DIALOGENTITY_H

#include <queue>

#include "EntityInterface.h"
#include "MessageEntity.h"
#include "CipherKey.h"

class DialogEntity : public EntityInterface
{
public:
    DialogEntity();
    DialogEntity(const EntityId peerId,
                 const QString peerName = QString(),
                 const CipherKey &localPrivateKey = CipherKey(),
                 const CipherKey &localPublicKey = CipherKey(),
                 const CipherKey &remotePublicKey = CipherKey());
    
    EntityId                          getPeerId     () const;
    const QString&                    getPeerName   () const;
    const MessageEntity&              getLastMessage() const;
    const std::vector<MessageEntity>& getMessages   () const;
    
    bool isEncrypted() const;
    
    CipherKey getLocalPrivateKey() const;
    CipherKey getLocalPuplicKey () const;
    CipherKey getRemotePublicKey() const;
    
    void resetKeys();
    
    MessageEntity takeBufferedMessage();
    
    bool appendBufferedMessage(const MessageEntity &message);
    bool appendMessage        (const MessageEntity &message);

    bool setPeerName (const QString &peerName);
    bool setLocalKeys(const CipherKey &localPublicKey,
                      const CipherKey &localPrivateKey);
    bool setRemoteKey(const CipherKey &remotePublicKey);
    
    bool isInitChecked();
    void setIsInitChecked();
    
    virtual bool isValid() const override;
    
private:
    EntityId      m_peerId;
    QString       m_peerName;
    
    std::vector<MessageEntity> m_messages;
    std::queue<MessageEntity>  m_bufferedMessages;
    
    CipherKey m_localPrivateKey;
    CipherKey m_localPublicKey;
    CipherKey m_remotePublicKey;
    
    bool m_isInitChecked;
};

#endif // DIALOGENTITY_H
