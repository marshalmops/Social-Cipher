#ifndef DIALOGMESSAGESMODEL_H
#define DIALOGMESSAGESMODEL_H

#include <vector>
#include <QAbstractListModel>

#include "Error.h"
#include "NetworkDialogMessagesFacadeInterface.h"
#include "DialogEntity.h"
#include "MessageEntity.h"
#include "EncoderInterface.h"

class DialogMessagesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString peerName READ getPeerName NOTIFY messagesSet)
    
public:
    constexpr static const char*   C_COMMAND_START  = "__";
    constexpr static const uint8_t C_COMMAND_LENGTH = 4;
    
    enum CommandCode : uint8_t {
        CC_INVALID = 0,
        CC_START_ENCRYPTION_INIT,
        CC_END_ENCRYPTION_INIT,
        CC_RESET_ENCRYPTION
    };
    
    static const QHash<CommandCode, QString>& getCommandsHash();
    
    static const CommandCode getCommandCodeByString(const QString &command);
    static const QString     getStringByCommandCode(const CommandCode code);
    
    constexpr static const char* C_MESSAGE_ORIGIN            = "isLocalMessage";
    constexpr static const char* C_MESSAGE_CONTENT           = "messageContent";
    constexpr static const char* C_MESSAGE_TIME              = "messageTime";
    constexpr static const char* C_MESSAGE_ENCRYPTION_STATUS = "isMessageEncypted";
    
    enum UserRolesIds : uint16_t {
        URI_MESSAGE_ORIGIN = Qt::UserRole + 1,
        URI_MESSAGE_CONTENT,
        URI_MESSAGE_TIME,
        URI_MESSAGE_ENCRYPTION_STATUS,
        URI_COUNT
    };
    
    explicit DialogMessagesModel(/*std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogMessagesFacade,*/
                                 std::unique_ptr<EncoderInterface> &&encoder,
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
    
public slots:
    void setDialogMessagesModelFacades(std::shared_ptr<NetworkDialogMessagesFacadeInterface> dialogsMessagesFacade);
    
    void setDialogMessages  (std::shared_ptr<DialogEntity> dialog);
    void unsetDialogMessages();     // qml signal handler.
    
    void sendMessage  (const QString message);      // qml.
    void appendMessage(const MessageEntity message);
    
    void startEncryption(); // qml
    void resetEncryption(); // qml
    
private:
    void resetModelContent();
    
    void insertMessageRow(const MessageEntity &message);
    
    CommandCode scanStringForCommand(const QString str) const;
    void processCommand(const CommandCode command,
                        const QString     &content);
    MessageEntity generateCommandMessage(const CommandCode command,
                                         const QString     &content = QString()) const;
    
    QByteArray prepareEncodedString(const QString str)      const;
    QString    prepareDecodedBytes (const QByteArray bytes) const;
    
    std::shared_ptr<DialogEntity> m_dialog;
    
    std::unique_ptr<EncoderInterface>                     m_encoder;
    std::shared_ptr<NetworkDialogMessagesFacadeInterface> m_dialogsMessagesFacade;
};

#endif // DIALOGMESSAGESMODEL_H
