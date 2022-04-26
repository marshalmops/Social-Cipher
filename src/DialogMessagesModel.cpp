#include "DialogMessagesModel.h"
#include "DialogMessagesModel.h"

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

QVariant DialogMessagesModel::data(const QModelIndex &index, int role) const
{
    QVariant data{};
    
    if (!index.isValid() || role < 0 || index.row() >= rowCount()) 
        return data;
    
    auto &messages   = m_dialog->getMessages();
    auto &curMessage = messages[index.row()];
    
    if (role <= Qt::UserRole || role >= UserRolesIds::URI_COUNT) 
        return data;
    
    switch (role) {
        case UserRolesIds::URI_MESSAGE_ORIGIN:            data = curMessage.isLocal(); break;
        case UserRolesIds::URI_MESSAGE_CONTENT:           data = curMessage.getText(); break;
        case UserRolesIds::URI_MESSAGE_TIME:              data = curMessage.getTime().toString("hh:mm"); break;
        case UserRolesIds::URI_MESSAGE_ENCRYPTION_STATUS: data = curMessage.isEncrypted(); break;
    }
    
    return data;
}

QHash<int, QByteArray> DialogMessagesModel::roleNames() const
{
    static QHash<int, QByteArray> rolesHash = {
        {UserRolesIds::URI_MESSAGE_ORIGIN,            C_MESSAGE_ORIGIN},
        {UserRolesIds::URI_MESSAGE_CONTENT,           C_MESSAGE_CONTENT},
        {UserRolesIds::URI_MESSAGE_TIME,              C_MESSAGE_TIME},
        {UserRolesIds::URI_MESSAGE_ENCRYPTION_STATUS, C_MESSAGE_ENCRYPTION_STATUS}
    };
    
    return rolesHash;
}

QString DialogMessagesModel::getPeerName()
{
    if (!m_dialog.get()) return QString();
    
    return m_dialog->getPeerName();
}

void DialogMessagesModel::sendMessage(const QString message)
{
    QString processedMessage;
    bool    isEncrypted{m_dialog->isEncrypted()};
    
    if (isEncrypted) {
        processedMessage = prepareDecodedBytes(m_encoder->encodeString(message, m_dialog->getRemotePublicKey()));
    
        if (processedMessage.isEmpty()) 
            emit errorOccured(Error{"Message encoding fail!", true});
        
    } else
        processedMessage = message;
    
    MessageEntity messageToSend{processedMessage, isEncrypted, 0, m_dialog->getPeerId(), 0, QDateTime::currentDateTime()};
    
    if (!messageToSend.isValid()) return;
    
    auto err = m_dialogsMessagesFacade->sendMessage(messageToSend, m_dialog->getPeerId());
    
    if (err.isValid()) {
        emit errorOccured(err);
        
        return;
    }
    
    insertMessageRow(isEncrypted ? MessageEntity{message, isEncrypted, 0, m_dialog->getPeerId(), 0, QDateTime::currentDateTime()} : messageToSend);
}

void DialogMessagesModel::setDialogMessages(std::shared_ptr<DialogEntity> dialog)
{
    m_dialog = dialog;
    
    auto processedMessagesCount = rowCount();
    
    if (processedMessagesCount > 0) {
        beginInsertRows(QModelIndex(), 0, processedMessagesCount - 1);
        endInsertRows();
    }
    
    MessageEntity curBufferedMessage;
    
    while ((curBufferedMessage = m_dialog->takeBufferedMessage()).isValid()) {
        appendMessage(curBufferedMessage);
    }
    
    m_dialog->setIsInitChecked();
    
    emit messagesSet();
}

void DialogMessagesModel::unsetDialogMessages()
{
    resetModelContent();
    
    emit messagesUnset();
}

void DialogMessagesModel::appendMessage(const MessageEntity message)
{
    if (!message.isValid()) return;
    
//    QString messageText = message.getText();
    
    NetworkDialogMessagesFacadeInterface::CommandCode command;
    QString content;
    
    if (!m_dialogsMessagesFacade->getCommandByMessage(message, command, content)) {
        return;
    }
    
//    CommandCode command = scanStringForCommand(messageText);
    
    if (command != NetworkDialogMessagesFacadeInterface::CommandCode::CC_INVALID && m_dialog->isInitChecked()) {
        //processCommand(command, messageText.mid(strlen(C_COMMAND_START) + C_COMMAND_LENGTH));
        processCommand(command, content);
        
        return;
    }
    
    QString messageText = message.getText();
    
    if (m_dialog->isEncrypted()) {
        messageText = m_encoder->decodeString(prepareEncodedString(messageText), m_dialog->getLocalPrivateKey());
    
        if (messageText.isEmpty()) emit errorOccured(Error{"Message decoding fail!"});
    }
    
    insertMessageRow(MessageEntity{messageText, m_dialog->isEncrypted(), message.getFromId(), message.getPeerId(), message.getMessageId(), message.getTime()});
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

void DialogMessagesModel::setDialogMessagesModelFacades(std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogsMessagesFacade)
{
    m_dialogsMessagesFacade = dialogsMessagesFacade;
}

void DialogMessagesModel::resetModelContent()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());
    
    m_dialog.reset();
    
    endRemoveRows();
}

void DialogMessagesModel::insertMessageRow(const MessageEntity &message)
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
