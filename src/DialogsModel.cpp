#include "DialogsModel.h"

DialogsModel::DialogsModel(QObject *parent)
    : QAbstractListModel    {parent},
      m_dialogsFacade       {/*dialogsFacade.release()*/},
      m_dialogMessagesFacade{/*dialogMessagesFacade*/}
{
    
}

int DialogsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    
    return m_dialogs.size();
}

QVariant DialogsModel::data(const QModelIndex &index, int role) const
{   
    QVariant data{};
    
    if (!index.isValid() || role < 0 || index.row() >= rowCount()) 
        return data;
    if (role <= Qt::UserRole || role >= UserRolesIds::URI_COUNT) 
        return data;
    
    auto &curDialog      = m_dialogs[index.row()];//curDialogId];qInfo() << curDialogId;
    auto &curLastMessage = curDialog->getLastMessage();
    
    if (!curLastMessage.get()) return QVariant{};
    
    switch (role) {
        case UserRolesIds::URI_DIALOG_PEER_ID:           data = curDialog->getPeerId();                            break;
        case UserRolesIds::URI_DIALOG_PEER_NAME:         data = curDialog->getPeerName();                          break;
        case UserRolesIds::URI_DIALOG_LAST_MESSAGE_TEXT: data = curLastMessage->getText();                         break;
        case UserRolesIds::URI_DIALOG_LAST_MESSAGE_TIME: data = curLastMessage->getTime().toString("dd.MM hh:mm"); break;
    }
    
    return data;
}

QHash<int, QByteArray> DialogsModel::roleNames() const
{
    static QHash<int, QByteArray> rolesHash = {
        {URI_DIALOG_PEER_ID,           C_DIALOG_PEER_ID},
        {URI_DIALOG_PEER_NAME,         C_DIALOG_PEER_NAME},
        {URI_DIALOG_LAST_MESSAGE_TEXT, C_DIALOG_LAST_MESSAGE_TEXT},
        {URI_DIALOG_LAST_MESSAGE_TIME, C_DIALOG_LAST_MESSAGE_TIME}
    };
    
    return rolesHash;
}

void DialogsModel::openDialog(const quint64 dialogIndex)
{
    std::shared_ptr<DialogEntityBase> curDialog;
    
    if (!getDialogById(dialogIndex, curDialog)) {
        emit errorOccured(Error{"Choosen dialog can't be found!"});
        
        return;
    }
    
    m_curDialogId = dialogIndex;
    
    emit dialogSet(curDialog);
}

void DialogsModel::closeDialog()
{
    resetCurrentDialog();
}

void DialogsModel::resetCurrentDialog()
{
    m_curDialogId = 0;
}

void DialogsModel::newMessagesOccured(std::vector<std::shared_ptr<MessageEntityBase>> messages)
{
    for (auto messageIter = messages.begin(); messageIter != messages.end(); ++messageIter) {
        std::shared_ptr<MessageEntityBase>& message{*messageIter};
        
        if (message->isLocal()) continue;
        
        MessageEntityBase::EntityId fromId = message->getFromId();
        
        std::shared_ptr<DialogEntityBase> curDialog;
        
        if (!getDialogById(fromId, curDialog)) {
            QString peerName{};
            Error   err     {};
            
            if ((err = m_dialogsFacade->getPeerName(fromId, peerName)).isValid()) {
                emit errorOccured(err);
                
                return;
            }
            
            std::unique_ptr<DialogEntityBase> newDialog{std::make_unique<DialogEntityBase>(fromId, peerName)};
            
            newDialog->appendBufferedMessage(message);
            
            insertDialogRow(newDialog, false);
            
            continue;
        }
        
        if (fromId == m_curDialogId) {
            emit activeDialogMessageReceived(message);
            
        } else {
            if (!curDialog->appendBufferedMessage(message)) 
                emit errorOccured(Error{"New message appending error!", true});
        
            dialogPreviewChanged(curDialog->getPeerId());
        }
    }
}

void DialogsModel::initializeDialogs()
{
    if (!m_dialogsFacade->initialize()
     || !m_dialogMessagesFacade->initialize())
    {
        emit errorOccured(Error{"Dialogs model facades init error!", true});
        
        return;
    }
    
    std::vector<std::unique_ptr<DialogEntityBase>> dialogs;
    
    auto err = m_dialogsFacade->getDialogs(dialogs);
    
    if (err.isValid()) {
        emit errorOccured(err);
        
        return;
    }
    
    auto networkSettings = NetworkSettings::getInstance();
    
    for (auto dialogIter = dialogs.begin(); dialogIter != dialogs.end(); ++dialogIter) {
        auto &dialog{*dialogIter};
        
        if (dialog->getPeerId() == networkSettings->getLocalPeerId()) continue;
        
        if (dialog->getPeerName().isEmpty()) {
            QString peerName{};
            
            err = m_dialogsFacade->getPeerName(dialog->getPeerId(), peerName);
            
            if (err.isValid()) {
                emit errorOccured(Error{"Getting peer name error!"});
                
                return;
            }
            
            if (!dialog->setPeerName(peerName)) {
                emit errorOccured(Error{"Setting peer name error!"});
                
                return;
            }
        }
        
        std::vector<std::shared_ptr<MessageEntityBase>> curDialogMessages;
        
        err = m_dialogMessagesFacade->getDialogMessages(dialog->getPeerId(), curDialogMessages);
        
        if (err.isValid()) {
            emit errorOccured(err);
            
            return;
        }
        
        for (auto messagesIter = curDialogMessages.begin(); messagesIter != curDialogMessages.end(); ++messagesIter) {
            if (!dialog->appendBufferedMessage(*messagesIter)) {
                emit errorOccured(Error{"Messages appending error!", true});
                
                return;
            }
        }
        
        if (!dialog->isValid()) {
            emit errorOccured(Error{"Loaded dialogs are not correct!", true});
            
            return;
        }
        
        insertDialogRow(dialog);
        
        QThread::currentThread()->msleep(500);
    }
    
    emit dialogsInitialized();
}

void DialogsModel::unsetDialogs()
{
    resetDialogsEncryption();
    
    resetModelContent();
    
    emit dialogsUnset();
}

void DialogsModel::dialogPreviewChanged(const EntityInterface::EntityId id)
{
    uint32_t curDialogRow{0};
    
    if (!getIndexOfDialogById(id, curDialogRow)) {
        emit errorOccured(Error{"Dialog updating error!", true});
        
        return;
    }
    
    emit dataChanged(index(curDialogRow, 0), index(curDialogRow, 0));
}

void DialogsModel::currentDialogMessageInserted()
{
    dialogPreviewChanged(m_curDialogId);
}

void DialogsModel::resetDialogs()
{
    resetDialogsEncryption();
    
    emit dialogsReset();
}

void DialogsModel::setDialogsModelNetworkInterface(NetworkDialogsFacadeInterface *dialogsFacade,
                                                   std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogMessagesFacade)
{
    m_dialogsFacade        = std::unique_ptr<NetworkDialogsFacadeInterface>(dialogsFacade);
    m_dialogMessagesFacade = dialogMessagesFacade;

}

void DialogsModel::resetModelContent()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());
    
    m_dialogs.clear();
    
    endRemoveRows();
}

void DialogsModel::resetDialogsEncryption()
{
    foreach (const auto &dialog, m_dialogs) {
        if (!dialog->isEncrypted()) continue;
        
        auto err = m_dialogMessagesFacade->sendCommand(NetworkDialogMessagesFacadeInterface::CommandCode::CC_RESET_ENCRYPTION,
                                                       QString(),
                                                       dialog->getPeerId());
        
        if (err.isValid()) emit errorOccured(err);
    }
}

void DialogsModel::insertDialogRow(std::unique_ptr<DialogEntityBase> &dialog, const bool toBack)
{
    auto curDialogsCount = rowCount();
    
    if (toBack) {
        emit beginInsertRows(QModelIndex(), curDialogsCount, curDialogsCount);
        
        m_dialogs.push_back(std::shared_ptr<DialogEntityBase>(dialog.release()));
        
    } else {
        emit beginInsertRows(QModelIndex(), 0, 0);
        
        *(m_dialogs.emplace(m_dialogs.begin())) = std::shared_ptr<DialogEntityBase>(dialog.release());
    }
    
    emit endInsertRows();
}

bool DialogsModel::getIndexOfDialogById(const EntityInterface::EntityId dialogId,
                                        uint32_t &index) const
{
    uint32_t curIndex{0};
    
    foreach (const auto &curDialog, m_dialogs) {
        if (dialogId == curDialog->getPeerId()) {
            index = curIndex;
            
            return true;
        }
        
        ++curIndex;
    }
    
    return false;
}

bool DialogsModel::getIdOfDialogByIndex(const uint32_t index,
                                        EntityInterface::EntityId &dialogId) const
{
    auto curIndex{0};
    
    foreach (const auto &curDialog, m_dialogs) {
        if (curIndex == index) {
            dialogId = curDialog->getPeerId();
        
            return true;
        }
        
        ++curIndex;
    }
    
    return false;
}

bool DialogsModel::getDialogById(const EntityInterface::EntityId dialogId,
                                 std::shared_ptr<DialogEntityBase> &dialog) const
{
    foreach (const auto &curDialog, m_dialogs) {
        if (curDialog->getPeerId() == dialogId) {
            dialog = curDialog;
            
            return true;
        }
    }
    
    return false;
}
