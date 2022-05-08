import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/components"

ViewBase {
    ListView {
        id: messagesListView
        
        anchors.fill: parent
        
        model: dialogsModel
        
        headerPositioning: ListView.OverlayHeader
        
        header: Rectangle {
            height: 25
            width: parent.width
            z: 3
            
            color: "white"
            
            anchors.right: parent.right
            anchors.left:  parent.left
            
            BackButton {
                id: _backButton
                
                height: parent.height
                anchors.left: parent.left
                
                onClicked: {
                    dialogsModel.unsetDialogs();
                }
            }
            
            TitleText {
                anchors.left: _backButton.right
                anchors.right: parent.right
                
                verticalAlignment:   Text.AlignVCenter
                
                text: qsTr("Dialogs")
            }
        }
        
        delegate: DialogDelegate {
            width: messagesListView.width
            
            lastMessageText: dialogLastMessageText
            peerName: dialogPeerName
            lastMessageDate: dialogLastMessageTime
            peerId: dialogPeerId
            
            onDialogClicked: function(peerId) {
                appExecManager.startLoading();
                dialogsModel.openDialog(peerId);
            }
        }
    }
    
    Keys.onBackPressed: {
        dialogsModel.unsetDialogs();
    }
}
