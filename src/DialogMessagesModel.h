#ifndef DIALOGMESSAGESMODEL_H
#define DIALOGMESSAGESMODEL_H

#include <vector>
#include <unordered_map>
#include <QAbstractListModel>

#include "Error.h"

#include "NetworkDialogMessagesFacadeInterface.h"

#include "DialogEntityBase.h"
#include "MessageEntityBase.h"

#include "EncoderInterface.h"

#include "AttachmentManagerBase.h"

struct QUrlHasher;

class DialogMessagesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString peerName READ getPeerName NOTIFY messagesSet)
    
public:
    constexpr static const char* C_MESSAGE_ORIGIN            = "isLocalMessage";
    constexpr static const char* C_MESSAGE_CONTENT           = "messageContent";
    constexpr static const char* C_MESSAGE_TIME              = "messageTime";
    constexpr static const char* C_MESSAGE_ENCRYPTION_STATUS = "isMessageEncypted";
    constexpr static const char* C_MESSAGE_ATTACHMENTS       = "messageAttachments";
    
    enum UserRolesIds : uint16_t {
        URI_MESSAGE_ORIGIN = Qt::UserRole + 1,
        URI_MESSAGE_CONTENT,
        URI_MESSAGE_TIME,
        URI_MESSAGE_ENCRYPTION_STATUS,
        URI_MESSAGE_ATTACHMENTS,
        URI_COUNT
    };
    
    explicit DialogMessagesModel(std::unique_ptr<EncoderInterface> &&encoder,
                                 QObject *parent = nullptr);
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    
    Q_INVOKABLE QString getPeerName();
signals:
    void errorOccured(const Error error);
    
    void messagesSet  ();
    void messagesUnset();
    
    void messageRowInserted();
    
    void encryptionStarted();
    void encryptionReset();
    
    void messageSent();
    
public slots:
    void setDialogMessagesModelNetworkInterface(std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogsMessagesFacade,
                                                std::shared_ptr<AttachmentManagerBase> attachmentsManager);
    
    void setDialogMessages  (std::shared_ptr<DialogEntityBase> dialog);
    void unsetDialogMessages();     // qml signal handler.
    
    void sendMessage  (const QString message,
                       const QVector<QUrl> attachmentFilesPaths); // qml.
    void appendMessage(std::shared_ptr<MessageEntityBase> message);
    
    void startEncryption(); // qml
    void resetEncryption(); // qml
    
private:
    void resetModelContent();
    
    void insertMessageRow(std::shared_ptr<MessageEntityBase> &message);
    
    void processCommand(const NetworkDialogMessagesFacadeInterface::CommandCode command,
                        const QString     &content);

    QByteArray prepareEncodedString(const QString str)      const;
    QString    prepareDecodedBytes (const QByteArray bytes) const;
    
    // sending message utils:
    
    bool loadLocalAttachmentsFilesContent(const QVector<QUrl> attachmentFilesPaths,
                                          std::unordered_map<QUrl, std::unique_ptr<AttachmentContentBase>, QUrlHasher> &attachmentsUrlsContentHash);
    bool encryptMessageTextAndAttachments(const QString &sourceMessage,
                                          std::unordered_map<QUrl, std::unique_ptr<AttachmentContentBase>, QUrlHasher> &attachmentsUrlsContentHash,
                                          QString &processedMessage);
    bool uploadAttachments(std::unordered_map<QUrl, std::unique_ptr<AttachmentContentBase>, QUrlHasher> &attachmentsUrlsContentHash,
                           std::unordered_map<std::shared_ptr<AttachmentEntityBase>, QUrl> &uploadedAttachmentsEntitiesLocalUrlsHash,
                           std::vector<std::shared_ptr<AttachmentEntityBase>> &uploadedAttachmentsEntitiesCopy);
    bool storeUploadedAttachments(const std::unordered_map<std::shared_ptr<AttachmentEntityBase>, QUrl> &uploadedAttachmentsEntitiesLocalUrlsHash);
    
    // appending message utils:
    
    bool checkMessageForCommand(const std::shared_ptr<MessageEntityBase> &message);
    bool downloadAttachments(const std::vector<std::shared_ptr<AttachmentEntityBase>> &attachments,
                             std::unordered_map<AttachmentEntityBase::EntityId, std::pair<std::shared_ptr<AttachmentEntityBase>, std::unique_ptr<AttachmentContentBase>>> &attachmentsEntitiesContentHash,
                             std::vector<std::shared_ptr<AttachmentEntityBase>> &downloadedAttachmentsEntitiesCopy);
    bool decryptMessageTextAndAttachments(std::unordered_map<AttachmentEntityBase::EntityId, std::pair<std::shared_ptr<AttachmentEntityBase>, std::unique_ptr<AttachmentContentBase>>> &attachmentsEntitiesContentHash,
                                          QString &messageText);
    bool storeDownloadedAttachments(const std::unordered_map<AttachmentEntityBase::EntityId, std::pair<std::shared_ptr<AttachmentEntityBase>, std::unique_ptr<AttachmentContentBase>>> &attachmentsEntitiesContentHash);
    
    std::shared_ptr<DialogEntityBase> m_dialog;
    
    std::unique_ptr<EncoderInterface>                     m_encoder;
    std::shared_ptr<NetworkDialogMessagesFacadeInterface> m_dialogsMessagesFacade;
    std::shared_ptr<AttachmentManagerBase>                m_attachmentManager;
};

#endif // DIALOGMESSAGESMODEL_H
