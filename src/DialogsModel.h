#ifndef DIALOGSMODEL_H
#define DIALOGSMODEL_H

#include <QAbstractListModel>
#include <QThread>

#include "Error.h"

#include "NetworkDialogsFacadeInterface.h"
#include "NetworkDialogMessagesFacadeInterface.h"

#include "DialogEntityBase.h"

class DialogsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    constexpr static const char* C_DIALOG_PEER_ID           = "dialogPeerId";
    constexpr static const char* C_DIALOG_PEER_NAME         = "dialogPeerName";
    constexpr static const char* C_DIALOG_LAST_MESSAGE_TEXT = "dialogLastMessageText";
    constexpr static const char* C_DIALOG_LAST_MESSAGE_TIME = "dialogLastMessageTime";
    
    enum UserRolesIds : uint16_t {
        URI_DIALOG_PEER_ID = Qt::UserRole + 1,
        URI_DIALOG_PEER_NAME,
        URI_DIALOG_LAST_MESSAGE_TEXT,
        URI_DIALOG_LAST_MESSAGE_TIME,
        URI_COUNT
    };
    
    DialogsModel(QObject *parent = nullptr);
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    
signals:
    void errorOccured(const Error error);
    
    void dialogSet                  (std::shared_ptr<DialogEntityBase> dialog);
    void activeDialogMessageReceived(std::shared_ptr<MessageEntityBase> message);
    
    void dialogsInitialized();
    void dialogsUnset      ();
    
    void dialogsReset();
    
public slots:
    void setDialogsModelNetworkInterface(NetworkDialogsFacadeInterface *dialogsFacade,
                                         std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogMessagesFacade);
    
    void openDialog (const quint64 dialogIndex);     // qml
    void closeDialog();
    
    void newMessagesOccured(std::vector<std::shared_ptr<MessageEntityBase>> messages);
    
    void initializeDialogs();
    void unsetDialogs     ();   // qml signal handler.
    
    void dialogPreviewChanged        (const DialogEntityBase::EntityId id);
    void currentDialogMessageInserted();
    
    void resetDialogs();
    
private:
    void resetCurrentDialog();
    void resetModelContent ();
    void resetDialogsEncryption();
    
    void insertDialogRow(std::unique_ptr<DialogEntityBase> &dialog);
    
    bool getIndexOfDialogById(const DialogEntityBase::EntityId dialogId, 
                              uint32_t &index) const;
    bool getIdOfDialogByIndex(const uint32_t index,
                              EntityInterface::EntityId &dialogId) const;
    bool getDialogById(const DialogEntityBase::EntityId dialogId,
                       std::shared_ptr<DialogEntityBase> &dialog) const;
    
    DialogEntityBase::EntityId m_curDialogId;
    
    /*QHash<DialogEntity::EntityId, */std::vector<std::shared_ptr<DialogEntityBase>> m_dialogs;
    
    std::unique_ptr<NetworkDialogsFacadeInterface>        m_dialogsFacade;
    std::shared_ptr<NetworkDialogMessagesFacadeInterface> m_dialogMessagesFacade;
};

#endif // DIALOGSMODEL_H
