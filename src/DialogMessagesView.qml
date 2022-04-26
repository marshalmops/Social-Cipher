import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/components"

ViewBase {
    id: _root
    
    readonly property int messageWidthBreakpoint: 200
    property bool isEncrypted: false
    
    ListView {
        id: messagesListView
        
        anchors.fill: parent
        spacing: 5
        
        property int chosenMessageIndex: -1
        
//        onChosenMessageIndexChanged: {
            
//        }
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
                    messagesListView.chosenMessageIndex = -1;
                    
                    dialogMessagesModel.unsetDialogMessages();
                }
            }
            
            Rectangle {
                id: _headerTitleWrapper
                
                height: parent.height
                anchors.left: _backButton.right
                anchors.right: parent.right
                
                TitleText {
                    id: _headerTitle
                    
                    verticalAlignment:   Text.AlignVCenter
                    
                    text: dialogMessagesModel.peerName
                }
            }
        }
        
        delegate: MessageDelegate {
            width: messagesListView.width
            
            messageRectWidth: (messagesListView.width > messageWidthBreakpoint ? messagesListView.width * 0.8 : messagesListView.width)
            
            messageText:            messageContent
            messageDate:            messageTime
            isLocal:                isLocalMessage
            messageEncryptedStatus: isMessageEncypted
            messageIndex:           index
            
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
        
        footer: Rectangle {
            id: _listViewFooter
            
            width: parent.width
            height: _buttonsColumn.height + (messagesListView.isChosenMessageIndexValid() ? (_toolbar.height + _footerControlsColumn.spacing) : 0)
            z: 3
            
            border {
                width: 1
                color: "grey"
            }
            
            function handleMessageSending() {
                if (_sendingMessage.inputText.length == 0) return;
                
                dialogMessagesModel.sendMessage(_sendingMessage.inputText);
                _sendingMessage.cleanUp();
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
                    //anchors.fill: parent
                    Layout.fillWidth: true
                    
                    FlickableTextArea {
                        id: _sendingMessage
                        
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        
                        placeholderText: qsTr("Enter your message")
                        
                        onReturnPressed: {
                            handleMessageSending();
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
                                handleMessageSending();
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
                    }
                }
            }
        }
    }
    
    TextEdit {
        id: _copyBuffer
        
        visible: false
    }
    
    Keys.onBackPressed: {
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
