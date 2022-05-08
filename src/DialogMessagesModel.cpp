#include "DialogMessagesModel.h"
#include "DialogMessagesModel.h"

struct QUrlHasher {
    std::size_t operator()(const QUrl &url) const {
        return qHash(url);
    }
};
    

DialogMessagesModel::DialogMessagesModel(std::unique_ptr<EncoderInterface> &&encoder,
                                         QObject *parent)
    : QAbstractListModel     {parent},
      m_dialogsMessagesFacade{},
      m_encoder              {encoder.release()}
{
    
}

int DialogMessagesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_dialog.get()) return 0;
    
    return m_dialog->getMessages().size();
}

QVariant DialogMessagesModel::data(const QModelIndex &index, 
                                   int role) const
{
    QVariant data{};
    
    if (!index.isValid() || role < 0 || index.row() >= rowCount()) 
        return data;
    
    auto &messages   = m_dialog->getMessages();
    auto &curMessage = messages[index.row()];
    
    if (role <= Qt::UserRole || role >= UserRolesIds::URI_COUNT) 
        return data;
    
    switch (role) {
        case UserRolesIds::URI_MESSAGE_ORIGIN:            data = curMessage->isLocal();                   break;
        case UserRolesIds::URI_MESSAGE_CONTENT:           data = curMessage->getText();                   break;
        case UserRolesIds::URI_MESSAGE_TIME:              data = curMessage->getTime().toString("hh:mm"); break;
        case UserRolesIds::URI_MESSAGE_ENCRYPTION_STATUS: data = curMessage->isEncrypted();               break;
        case UserRolesIds::URI_MESSAGE_ATTACHMENTS: {
            const auto &curMessageAttachments{curMessage->getAttachments()};
            QList<QVariant> attachmentsLocalFilesLinks{};
            
            foreach (const auto &attachment, curMessageAttachments) {
                std::unique_ptr<AttachmentLocalFileLink> curAttachmentLocalFileLink{};
                Error err{};
                
                if ((err = m_attachmentManager->getAttachmentLocalFileLinkById(attachment->getAttachmentId(), curAttachmentLocalFileLink)).isValid())
                    return QVariant{};
                
                attachmentsLocalFilesLinks.append(QVariant::fromValue<AttachmentLocalFileLink>(*curAttachmentLocalFileLink.release()));
            }
            
            data = attachmentsLocalFilesLinks;
        
            break;
        }
    }
    
    return data;
}

QHash<int, QByteArray> DialogMessagesModel::roleNames() const
{
    static QHash<int, QByteArray> rolesHash = {
        {UserRolesIds::URI_MESSAGE_ORIGIN,            C_MESSAGE_ORIGIN},
        {UserRolesIds::URI_MESSAGE_CONTENT,           C_MESSAGE_CONTENT},
        {UserRolesIds::URI_MESSAGE_TIME,              C_MESSAGE_TIME},
        {UserRolesIds::URI_MESSAGE_ENCRYPTION_STATUS, C_MESSAGE_ENCRYPTION_STATUS},
        {UserRolesIds::URI_MESSAGE_ATTACHMENTS,       C_MESSAGE_ATTACHMENTS}
    };
    
    return rolesHash;
}

QString DialogMessagesModel::getPeerName()
{
    if (!m_dialog.get()) return QString();
    
    return m_dialog->getPeerName();
}

void DialogMessagesModel::sendMessage(const QString message, 
                                      const QVector<QUrl> attachmentFilesPaths)
{
    QVector<QUrl> correctedAttachmentsFilesPaths{};
    
#ifdef Q_OS_WIN
    foreach (const QUrl &attachmentFilePath, attachmentFilesPaths) {
        QString filePath{attachmentFilePath.path()};
        
        if (filePath.first(1) == "/") {
            QUrl correctUrl{};
            
            correctUrl.setPath(filePath.mid(1));
            
            correctedAttachmentsFilesPaths += correctUrl;
            
        } else {
            correctedAttachmentsFilesPaths += attachmentFilePath;
        }
    }
#else
    correctedAttachmentsFilesPaths = attachmentFilesPaths;
#endif
    
    // loading local attachments bytes:
    
    std::unordered_map<QUrl, std::unique_ptr<AttachmentContentBase>, ::QUrlHasher> attachmentsUrlsContentHash{};
    
    Error error{};
    
    if (!loadLocalAttachmentsFilesContent(correctedAttachmentsFilesPaths, attachmentsUrlsContentHash))
        return;
    
    // encrypting:
    
    QString processedMessage{};
    bool    isEncrypted     {m_dialog->isEncrypted()};
    
    if (isEncrypted) {
        if (!encryptMessageTextAndAttachments(message, attachmentsUrlsContentHash, processedMessage))
            return;
        
    } else 
        processedMessage = message;
    
    // uploading attachments:
    
    std::unordered_map<std::shared_ptr<AttachmentEntityBase>, QUrl> uploadedAttachmentsEntitiesLocalUrlsHash{};
    std::vector<std::shared_ptr<AttachmentEntityBase>> uploadedAttachmentsEntitiesCopy{};
    
    if (!uploadAttachments(attachmentsUrlsContentHash, uploadedAttachmentsEntitiesLocalUrlsHash, uploadedAttachmentsEntitiesCopy))
        return;
    
    // uploadedAttachments storing to files:
    
    if (!storeUploadedAttachments(uploadedAttachmentsEntitiesLocalUrlsHash))
        return;
    
    // actual sending:
    
    std::shared_ptr<MessageEntityBase> messageToSend{std::make_shared<MessageEntityBase>(processedMessage,
                                                                                         uploadedAttachmentsEntitiesCopy,
                                                                                         isEncrypted,
                                                                                         0,
                                                                                         m_dialog->getPeerId(),
                                                                                         0,
                                                                                         QDateTime::currentDateTime())};
    
    if (!messageToSend->isValid()) return;
    
    auto err = m_dialogsMessagesFacade->sendMessage(messageToSend, m_dialog->getPeerId());
    
    if (err.isValid()) {
        emit errorOccured(err);
        
        return;
    }
    
    if (isEncrypted) messageToSend->setText(message);
    
    insertMessageRow(messageToSend);
    
    emit messageSent();
    
//    insertMessageRow(isEncrypted 
//                     ? std::make_unique<MessageEntityBase>(message, messageToSend->getAttachments(), isEncrypted, 0, m_dialog->getPeerId(), 0, QDateTime::currentDateTime())
//                     : std::move(messageToSend));
}

void DialogMessagesModel::setDialogMessages(std::shared_ptr<DialogEntityBase> dialog)
{
    m_dialog = dialog;
    
    auto processedMessagesCount = rowCount();
    
    if (processedMessagesCount > 0) {
        beginInsertRows(QModelIndex(), 0, processedMessagesCount - 1);
        endInsertRows();
    }
    
    std::shared_ptr<MessageEntityBase> curBufferedMessage;
    
    while ((curBufferedMessage = m_dialog->takeBufferedMessage()).get())
        if (curBufferedMessage->isValid())
            appendMessage(curBufferedMessage);
    
    m_dialog->setIsInitChecked();
    
    emit messagesSet();
}

void DialogMessagesModel::unsetDialogMessages()
{
    resetModelContent();
    
    emit messagesUnset();
}

void DialogMessagesModel::appendMessage(std::shared_ptr<MessageEntityBase> message)
{
    if (!message->isValid()) return;
    
    // checking for command:
    
    if (!checkMessageForCommand(message)) return;
    
    // downloading attachments:
    
    std::unordered_map<AttachmentEntityBase::EntityId, std::pair<std::shared_ptr<AttachmentEntityBase>, std::unique_ptr<AttachmentContentBase>>> attachmentsEntitiesContentHash{};
    std::vector<std::shared_ptr<AttachmentEntityBase>> downloadedAttachmentsEntitiesCopy{};
    const auto &attachments{message->getAttachments()};
    
    if (!downloadAttachments(attachments, attachmentsEntitiesContentHash, downloadedAttachmentsEntitiesCopy)) return;
    
    // decrypting:
    
    QString messageText{message->getText()};
    
    if (m_dialog->isEncrypted()) {
        if (!decryptMessageTextAndAttachments(attachmentsEntitiesContentHash, messageText)) 
            return;
        
        message->setEncryptedFlag();
    }
    
    // storing processed downloaded attachments:
    
    if (!storeDownloadedAttachments(attachmentsEntitiesContentHash))
        return;
    
    if (!message->setText(messageText)) return;
    if (!downloadedAttachmentsEntitiesCopy.empty())
        if (!message->setAttachments(downloadedAttachmentsEntitiesCopy)) return;
    
    insertMessageRow(message);
}

void DialogMessagesModel::startEncryption()
{
    m_dialog->resetKeys();
    
    CipherKey publicKey,
              privateKey;
    
    if (!m_encoder->generateKeys(privateKey, publicKey)) {
        emit errorOccured(Error{"Keys generating error!", true});
        
        return;
    }
    
    if (!m_dialog->setLocalKeys(publicKey, privateKey)) {
        emit errorOccured(Error{"Keys setting error!", true});
        
        return;
    }
    
    auto err = m_dialogsMessagesFacade->sendCommand(NetworkDialogMessagesFacadeInterface::CommandCode::CC_START_ENCRYPTION_INIT,
                                                    prepareDecodedBytes(publicKey.getBytes()),
                                                    m_dialog->getPeerId());
    
    if (err.isValid()) emit errorOccured(err);
}

void DialogMessagesModel::resetEncryption()
{
    if (!m_dialog->isEncrypted()) return;
    
    m_dialog->resetKeys();
    
    if (m_dialogsMessagesFacade->getLastError().isValid()) return;
    
    auto err = m_dialogsMessagesFacade->sendCommand(NetworkDialogMessagesFacadeInterface::CommandCode::CC_RESET_ENCRYPTION,
                                                    QString(),
                                                    m_dialog->getPeerId());
    
    if (err.isValid()) emit errorOccured(err);
    
    emit encryptionReset();
}

void DialogMessagesModel::setDialogMessagesModelNetworkInterface(std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogsMessagesFacade, 
                                                                 std::shared_ptr<AttachmentManagerBase> attachmentsManager)
{
    m_dialogsMessagesFacade = dialogsMessagesFacade;
    m_attachmentManager     = attachmentsManager;
}

void DialogMessagesModel::resetModelContent()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());
    
    m_dialog.reset();
    
    endRemoveRows();
}

void DialogMessagesModel::insertMessageRow(std::shared_ptr<MessageEntityBase> &message)
{
    auto curMessagesCount = rowCount();
    
    beginInsertRows(QModelIndex(), curMessagesCount, curMessagesCount);
    
    m_dialog->appendMessage(message);
    
    endInsertRows();
    
    emit messageRowInserted();
}

void DialogMessagesModel::processCommand(const NetworkDialogMessagesFacadeInterface::CommandCode command,
                                         const QString &content)
{
    switch (command) {
        case NetworkDialogMessagesFacadeInterface::CommandCode::CC_START_ENCRYPTION_INIT: {
            m_dialog->resetKeys();
        
            CipherKey publicKey,
                      privateKey;
            
            if (!m_encoder->generateKeys(privateKey, publicKey)) {
                emit errorOccured(Error{"Keys generating error!", true});
                
                return;
            }
            
            auto remotePublicKey {prepareEncodedString(content)};
            
            if (!m_dialog->setLocalKeys(publicKey, privateKey)
             || !m_dialog->setRemoteKey(CipherKey(remotePublicKey)))
            {
                emit errorOccured(Error{"Keys setting error!", true});
                
                return;
            }
            
            auto err = m_dialogsMessagesFacade->sendCommand(NetworkDialogMessagesFacadeInterface::CommandCode::CC_END_ENCRYPTION_INIT,
                                                            prepareDecodedBytes(publicKey.getBytes()),
                                                            m_dialog->getPeerId());
            
            if (err.isValid()) emit errorOccured(err);
            
            emit encryptionStarted();
            
            break;
        }
        case NetworkDialogMessagesFacadeInterface::CommandCode::CC_END_ENCRYPTION_INIT: {
            auto remotePublicKey {prepareEncodedString(content)};

            if (!m_dialog->setRemoteKey(CipherKey(remotePublicKey)))
            {
                emit errorOccured(Error{"Keys setting error!", true});
                
                return;
            }
            
            emit encryptionStarted();
            
            break;
        }
        case NetworkDialogMessagesFacadeInterface::CommandCode::CC_RESET_ENCRYPTION: {
            if (!m_dialog->isEncrypted()) return;
        
            m_dialog->resetKeys();
            
            emit encryptionReset();
            
            break;
        }
    }
}

QByteArray DialogMessagesModel::prepareEncodedString(const QString str) const
{
    return QByteArray::fromBase64(str.toUtf8());
}

QString DialogMessagesModel::prepareDecodedBytes(const QByteArray bytes) const
{
    return QString::fromUtf8(bytes.toBase64());
}

bool DialogMessagesModel::loadLocalAttachmentsFilesContent(const QVector<QUrl> attachmentFilesPaths,
                                                           std::unordered_map<QUrl, std::unique_ptr<AttachmentContentBase>, ::QUrlHasher> &attachmentsUrlsContentHash)
{
    foreach (const auto &attachmentPath, attachmentFilesPaths) {
        std::unique_ptr<AttachmentContentBase> curAttachmentContent{};
        Error err{};
        
        if ((err = m_attachmentManager->getAttachmentContentByLocalPath(attachmentPath, curAttachmentContent)).isValid()) {
            // file is removed before reading try case.
            
            emit errorOccured(err);
            
            return false;
        }
        
        attachmentsUrlsContentHash[attachmentPath] = std::move(curAttachmentContent);
    }
    
    return true;
}

bool DialogMessagesModel::encryptMessageTextAndAttachments(const QString &sourceMessage,
                                                           std::unordered_map<QUrl, std::unique_ptr<AttachmentContentBase>, ::QUrlHasher> &attachmentsUrlsContentHash,
                                                           QString &processedMessage)
{
    if ((processedMessage = prepareDecodedBytes(m_encoder->encodeBytes(sourceMessage.toUtf8(), m_dialog->getRemotePublicKey()))).isEmpty()) {
        emit errorOccured(Error{"Message encrypting error!", true});
        
        return false;
    }
    
    for (auto iter = attachmentsUrlsContentHash.begin(); iter != attachmentsUrlsContentHash.end(); ++iter) {
        auto &curAttachmentContent{iter->second};
        QByteArray encodedAttachmentBytes{};

        if ((encodedAttachmentBytes = m_encoder->encodeBytes(curAttachmentContent->getByteArray(), m_dialog->getRemotePublicKey())).isEmpty()) {
            emit errorOccured(Error{"Attachment encrypting error!", true});
            
            return false;
        }
        
        // last byte adding (source extension)... FIXME
        
        encodedAttachmentBytes += AttachmentDefinerBase::getContentExtensionIdByExtension(AttachmentDefinerBase::getExtensionByFilename(curAttachmentContent->getFileName()));
        
        QString fileNameWithoutExtension{curAttachmentContent->getFileName().split('.').first()};
        
        curAttachmentContent = std::make_unique<AttachmentContentBase>(QString{fileNameWithoutExtension + '.' + NetworkSettings::C_ENCODED_ATTACHMENT_EXTENSION},
                                                                       encodedAttachmentBytes);
    }
    
    return true;
}

bool DialogMessagesModel::uploadAttachments(std::unordered_map<QUrl, std::unique_ptr<AttachmentContentBase>, ::QUrlHasher> &attachmentsUrlsContentHash,
                                            std::unordered_map<std::shared_ptr<AttachmentEntityBase>, QUrl> &uploadedAttachmentsEntitiesLocalUrlsHash,
                                            std::vector<std::shared_ptr<AttachmentEntityBase>> &uploadedAttachmentsEntitiesCopy)
{
    for (auto iter = attachmentsUrlsContentHash.begin(); iter != attachmentsUrlsContentHash.end(); ++iter) {
        std::unique_ptr<AttachmentContentBase> &attachmentContent{iter->second};
        std::shared_ptr<AttachmentEntityBase>  uploadedAttachment{};
        Error err{};
        
        if ((err = m_attachmentManager->uploadNewAttachment(attachmentContent, uploadedAttachment)).isValid()) {
            emit errorOccured(err);
            
            continue;
        }
        
        uploadedAttachmentsEntitiesLocalUrlsHash[uploadedAttachment] = iter->first;
        uploadedAttachmentsEntitiesCopy.push_back(uploadedAttachment);
    }
    
    return true;
}

bool DialogMessagesModel::storeUploadedAttachments(const std::unordered_map<std::shared_ptr<AttachmentEntityBase>, QUrl> &uploadedAttachmentsEntitiesLocalUrlsHash)
{
    for (auto iter = uploadedAttachmentsEntitiesLocalUrlsHash.begin(); iter != uploadedAttachmentsEntitiesLocalUrlsHash.end(); ++iter) {
        auto &curUploadedAttachmentEntity      {iter->first};
        auto &curUploadedAttachmentLocalFileUrl{iter->second};
        
        std::unique_ptr<AttachmentLocalFileLink> curAttachmentLocalFileLink{};
        Error err{};
        
        if ((err = m_attachmentManager->storeAttachmentLocalFileUrlById(curUploadedAttachmentEntity->getAttachmentId(), curUploadedAttachmentLocalFileUrl, curAttachmentLocalFileLink)).isValid()) {
            emit errorOccured(err);
            
            return false;
        }
    }
    
    return true;
}

bool DialogMessagesModel::checkMessageForCommand(const std::shared_ptr<MessageEntityBase> &message)
{
    NetworkDialogMessagesFacadeInterface::CommandCode command;
    QString content;
    
    if (!m_dialogsMessagesFacade->getCommandByMessage(message, command, content))
        return false;
    
    if (command != NetworkDialogMessagesFacadeInterface::CommandCode::CC_INVALID && m_dialog->isInitChecked()) {
        processCommand(command, content);
        
        return false;
    }
    
    return true;
}

bool DialogMessagesModel::downloadAttachments(const std::vector<std::shared_ptr<AttachmentEntityBase>> &attachments,
                                              std::unordered_map<AttachmentEntityBase::EntityId, std::pair<std::shared_ptr<AttachmentEntityBase>, std::unique_ptr<AttachmentContentBase>>> &attachmentsEntitiesContentHash,
                                              std::vector<std::shared_ptr<AttachmentEntityBase>> &downloadedAttachmentsEntitiesCopy)
{
    foreach (const auto &attachment, attachments) {
        Error err{};
        std::unique_ptr<AttachmentContentBase> curAttachmentContent{};
        
        if ((err = m_attachmentManager->downloadNewAttachment(attachment, curAttachmentContent)).isValid()) {
            emit errorOccured(err);
            
            continue;
        }
        
        attachmentsEntitiesContentHash[attachment->getAttachmentId()] = std::pair(attachment, std::move(curAttachmentContent));
        downloadedAttachmentsEntitiesCopy.push_back(attachment);
    }
    
    return true;
}

bool DialogMessagesModel::decryptMessageTextAndAttachments(std::unordered_map<AttachmentEntityBase::EntityId, std::pair<std::shared_ptr<AttachmentEntityBase>, std::unique_ptr<AttachmentContentBase>>> &attachmentsEntitiesContentHash,
                                                           QString &messageText)
{
    messageText = QString::fromUtf8(m_encoder->decodeBytes(prepareEncodedString(messageText), m_dialog->getLocalPrivateKey()));

    if (messageText.isEmpty()) emit errorOccured(Error{"Message decoding fail!"});
    
    // attachments content decrypting process:
    
    for (auto iter = attachmentsEntitiesContentHash.begin(); iter != attachmentsEntitiesContentHash.end(); ++iter) {
        auto &attachmentContentToDecrypt{iter->second.second};
        
        QByteArray decryptedAttachmentBytes{};
        
        AttachmentDefinerBase::ContentExtensionId sourceAttachmentExtensionId{static_cast<AttachmentDefinerBase::ContentExtensionIdType>(attachmentContentToDecrypt->getByteArray().back())};
        
        if (sourceAttachmentExtensionId == AttachmentDefinerBase::ContentExtensionId::CEI_INVALID) {
            emit errorOccured(Error{"Decrypting attachment error!", true});
            
            return false;
        }
        
        QByteArray attachmentBytesWithoutExtensionId{attachmentContentToDecrypt->getByteArray().first(attachmentContentToDecrypt->getByteArray().length() - 1)};

        if ((decryptedAttachmentBytes = m_encoder->decodeBytes(attachmentBytesWithoutExtensionId, m_dialog->getLocalPrivateKey())).isEmpty()) {
            emit errorOccured(Error{"Decrypting attachment error!", true});
            
            return false;
        }
        
        QString fileNameWithoutExtension{attachmentContentToDecrypt->getFileName().split('.').first()};
        
        attachmentContentToDecrypt = std::make_unique<AttachmentContentBase>(QString{fileNameWithoutExtension + '.' + AttachmentDefinerBase::getExtensionByContentExtensionId(sourceAttachmentExtensionId)},
                                                                             decryptedAttachmentBytes);
    }
    
    return true;
}

bool DialogMessagesModel::storeDownloadedAttachments(const std::unordered_map<AttachmentEntityBase::EntityId, std::pair<std::shared_ptr<AttachmentEntityBase>, std::unique_ptr<AttachmentContentBase>>> &attachmentsEntitiesContentHash)
{
    for (auto iter = attachmentsEntitiesContentHash.begin(); iter != attachmentsEntitiesContentHash.end(); ++iter) {
        auto &attachmentEntityToStore    {iter->second.first};
        auto &curAttachmentContentToStore{iter->second.second};
        
        std::unique_ptr<AttachmentLocalFileLink> curAttachmentLocalFileLink{};
        Error err{};
        
        if ((err = m_attachmentManager->storeAttachmentContentInFileById(attachmentEntityToStore->getAttachmentId(), curAttachmentContentToStore, curAttachmentLocalFileLink)).isValid()) {
            emit errorOccured(err);
            
            return false;
        }
        
        // ...
    }
    
    return true;
}
