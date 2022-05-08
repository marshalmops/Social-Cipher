import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import "qrc:/components"

ViewBase {
    id: _root
    
    readonly property int messageWidthBreakpoint: 200
    property bool isEncrypted: false
    
    property var attachmentsUrls: []
    
    function handleBackButtonClick() {
        messagesListView.chosenMessageIndex = -1;
        _root.attachmentsUrls.splice(0);
        
        dialogMessagesModel.unsetDialogMessages();
    }
    
    ListView {
        id: messagesListView
        
        anchors.fill: parent
        spacing: 5
        
        property int chosenMessageIndex: -1

        function isChosenMessageIndexValid() {
            return chosenMessageIndex >= 0;
        }
        
        model: dialogMessagesModel
        
        headerPositioning: ListView.OverlayHeader
        
        header: Rectangle {
            height: _headerTitle.contentHeight + 6
            z: 3
            width: parent.width
            
            color: "white"
                
            BackButton {
                id: _backButton
                
                height: parent.height
                anchors.left: parent.left
                
                onClicked: {
                    _root.handleBackButtonClick();
                }
            }
            
            Rectangle {
                id: _headerTitleWrapper
                
                height: parent.height
                anchors.left: _backButton.right
                anchors.right: parent.right
                
                TitleText {
                    id: _headerTitle
                    
                    verticalAlignment: Text.AlignVCenter
                    
                    text: dialogMessagesModel.peerName
                }
            }
        }
        
        delegate: MessageDelegate {
            width: messagesListView.width
            
            messageRectWidth: (messagesListView.width > _root.messageWidthBreakpoint ? messagesListView.width * 0.8 : messagesListView.width)
            
            messageText:              messageContent
            messageDate:              messageTime
            isLocal:                  isLocalMessage
            messageEncryptedStatus:   isMessageEncypted
            messageIndex:             index
            messageAttachmentsToShow: messageAttachments
            
            onMessageChosen: function (index) {
                if (messagesListView.isChosenMessageIndexValid()) {
                    var curChosenMessage = messagesListView.itemAtIndex(messagesListView.chosenMessageIndex);
                    
                    curChosenMessage.isChosen = false;
                }
                
                messagesListView.chosenMessageIndex = index;
            }
            
            onResetMessageChoosing: function (index) {
                messagesListView.chosenMessageIndex = -1;
            }
        }
        
        footerPositioning: ListView.OverlayFooter
        
        footer:  Rectangle {
            id: _listViewFooter
            
            width: parent.width
            height: _buttonsColumn.height + (messagesListView.isChosenMessageIndexValid() ? (_toolbar.height + _footerControlsColumn.spacing) : 0) + _attachmentsListRow.height
            z: 3
            
            Component.onCompleted: {
                console.log(_listViewFooter.height + " (" + _listViewFooter.x + ", " + _listViewFooter.y + ")");
            }
            
            border {
                width: 1
                color: "grey"
            }
            
            function handleMessageSending() {
                if (_sendingMessage.inputText.length == 0 && _root.attachmentsUrls == []) return;
                
                dialogMessagesModel.sendMessage(_sendingMessage.inputText, _root.attachmentsUrls); // LOCAL FILE URLS DROPPING HERE!!!
                
                _sendingMessage.cleanUp();
                _root.attachmentsUrls = [];
            }
            
            ColumnLayout {
                id: _footerControlsColumn
                
                anchors.fill: parent
                
                RowLayout {
                    id: _toolbar
                    
                    Layout.fillWidth: true
                    spacing: 10
                    
                    visible: messagesListView.isChosenMessageIndexValid()
                    
                    Rectangle {
                        id: _copyButton
                        
                        implicitWidth: 30
                        implicitHeight: implicitWidth
                        radius: implicitWidth / 2
                        Layout.topMargin: _footerControlsColumn.spacing
                        Layout.leftMargin: parent.spacing
                        Layout.alignment: Qt.AlignVCenter
                        
                        border {
                            width: 1
                            color: "darkgrey" 
                        }
                        
                        Text {
                            anchors.fill: parent
                            
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            
                            text: qsTr("Copy")
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            
                            onClicked: {
                                var message = messagesListView.itemAtIndex(messagesListView.chosenMessageIndex);
                                
                                if (message == null) return;
                                
                                _copyBuffer.text = message.messageText;
                                _copyBuffer.selectAll();
                                _copyBuffer.copy();
                                
                                messagesListView.chosenMessageIndex = -1;
                                _copyBuffer.clear();
                                
                                // clearing chosen messages...
                                
                                message.isChosen = false;
                            }
                        }
                    }
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    FlickableTextArea {
                        id: _sendingMessage
                        
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        
                        placeholderText: qsTr("Enter your message")
                        
                        onReturnPressed: {
                            _listViewFooter.handleMessageSending();
                        }
                    }
                    
                    // encryption and sending buttons...
                    
                    Column {
                        id: _buttonsColumn
                        
                        height: _sendButton.height + _encryptButton.height + spacing
                        spacing: 5
                        padding: 5
                        
                        SimpleButton {
                            id: _sendButton
                            
                            text: qsTr("Send")
                            
                            width: 50
                            
                            onClicked: {
                                appExecManager.startLoading();
                                _listViewFooter.handleMessageSending();
                            }
                        }
                        
                        SimpleButton {
                            id: _encryptButton
                            
                            text: qsTr("Encrypt")
                            
                            width: 70
                            
                            onClicked: {
                                _encryptButton.enabled = false;
                                
                                dialogMessagesModel.startEncryption();
                                
                                _encryptionRetryTimer.start();
                            }
                            
                            Timer {
                                id: _encryptionRetryTimer
                                
                                interval: 5000
                                onTriggered: _encryptButton.enabled = true;
                            }
                        }
                        
                        SimpleButton {
                            id: _resetEncryptionButton
                            
                            text: qsTr("Reset")
                            enabled: isEncrypted
                            
                            width: 70
                            
                            onClicked: {
                                dialogMessagesModel.resetEncryption();
                            }
                        }
                        
                        SimpleButton {
                            id: _attachButton
                            
                            width: 70
                            
                            text: qsTr("Attach")
                            
                            onClicked: {
                                _fileDialog.visible = true;
                            }
                            
                        }
                    }
                }
                
                RowLayout {
                    // attachments list:
                    
                    id: _attachmentsListRow
                    
                    Layout.fillWidth: true
                    
                    Flickable {
                        id: _attachmentListFlickable
                        
                        implicitHeight: (_root.attachmentsUrls.length > 0 ? 50 : 0)
                        Layout.fillWidth:  true
                        
                        clip: true
                        
                        ListView {
                            id: _attachmentsListView
                            
                            anchors.fill: parent
                            spacing: 5
                            
                            model: _root.attachmentsUrls
                            
                            delegate: AttachmentDelegate {
                                width: _attachmentsListView.width
                                
                                attachmentIndex: index
                                attachmentName:  {
                                    return ('' + modelData).split('/').pop();
                                }
                                
                                onAttachmentCanceled: function (attachmentIndex) {
                                    // attachment removing:
                                    
                                    var attachmentsUrlsAfterRemoving = [];
                                    var attachmentObjIndexAfterRemoving = 0;
                                    
                                    for (var attachmentObjIndex in _root.attachmentsUrls) {
                                        if (attachmentObjIndex != attachmentIndex) {
                                            attachmentsUrlsAfterRemoving[attachmentObjIndexAfterRemoving] = _root.attachmentsUrls[attachmentObjIndex];
                                        
                                            attachmentObjIndexAfterRemoving += 1;
                                        }
                                    }
                                    
                                    _root.attachmentsUrls = attachmentsUrlsAfterRemoving;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    FileDialog {
        id: _fileDialog
        
        visible: false
        fileMode: FileDialog.OpenFiles
        
        onAccepted: {
//            for (let index in _fileDialog.selectedFiles) {
//                _root.attachmentsUrls.push(_fileDialog.selectedFiles[index]);
//            }
            
            _root.attachmentsUrls = _fileDialog.selectedFiles;
            
            _fileDialog.visible = false;
        }
        
        onRejected: {
            
            
            _fileDialog.visible = false;
        }
    }
    
    TextEdit {
        id: _copyBuffer
        
        visible: false
    }
    
    Keys.onBackPressed: {
        _root.handleBackButtonClick();
        
        dialogMessagesModel.unsetDialogMessages();
    }
    
    Connections {
        target: dialogMessagesModel
        
        function onMessageRowInserted() {
            messagesListView.positionViewAtEnd();
        }
        
        function onEncryptionStarted() {
            isEncrypted = true;
        }
        
        function onEncryptionReset() {
            isEncrypted = false;
        }
    }
}
