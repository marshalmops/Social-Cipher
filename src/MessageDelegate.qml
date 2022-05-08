import QtQuick 2.15
import QtQuick.Layouts 1.12

Rectangle {
    id: root
    
    height: _messageWrapper.height
    
    property string messageText:            ""
    property string messageDate:            ""
    property bool   isLocal:                false
    property bool   messageEncryptedStatus: false
    property var    messageAttachmentsToShow: []
    
    property int messageIndex: -1
    property bool isChosen: false
    
    default property int messageRectWidth
    
    signal messageChosen       (int index);
    signal resetMessageChoosing(int index);
    
    Component.onCompleted: {
        if (isLocal) {
            _messageWrapper.anchors.right = root.right;
            
        } else {
            _messageWrapper.anchors.left  = root.left;
            
        }
    }
    
    color: Qt.rgba(0, 0, 0, 0)
    
    Rectangle {
        id: _messageWrapper
        
        property int defaultSpacing: 5
        
        height: _messageEncryptionStatus.height + _messageText.contentHeight + _messageDate.contentHeight + _attachmentsListView.height + _messageWrapper.defaultSpacing * 5
        width: messageRectWidth
        
        border.width: 1
        border.color: "darkgrey"
        
        color: isChosen ? "lightgrey" : "white"
            
        MouseArea {
            anchors.fill: parent
            
            onDoubleClicked: {
                console.log(messageIndex);
                
                isChosen = true;
                
                messageChosen(messageIndex);
            }
            
            onClicked: {
                if (!isChosen) return;
                
                isChosen = false;
                
                resetMessageChoosing(messageIndex);
            }
        }
            
        Text {
            id: _messageText
            
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            topPadding: _messageWrapper.defaultSpacing
            rightPadding: 10
            leftPadding: 10
            
            wrapMode: Text.Wrap
            text: messageText
        }
        
        ListView {
            // attachments list:
            
            id: _attachmentsListView
            
            anchors.top: _messageText.bottom
            anchors.topMargin: _messageWrapper.defaultSpacing
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            height: (root.messageAttachmentsToShow.length * width)
            
            model: messageAttachmentsToShow
            clip: true
            
            delegate: AttachmentToShowDelegate {
                width: _attachmentsListView.width
                
                attachmentLocalFileLink: modelData
            }
        }
        
        Text {
            id: _messageDate
            
            anchors.top: _attachmentsListView.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            horizontalAlignment: (isLocal ? Text.AlignLeft : Text.AlignRight)
            topPadding: _messageWrapper.defaultSpacing
            rightPadding: 10
            leftPadding: 10
            
            text: messageDate
        }
        
        Rectangle {
            id: _messageEncryptionStatus
            
            width: 10
            height: width
            radius: width / 2
            
            anchors.top: _messageDate.bottom
            anchors.topMargin: _messageWrapper.defaultSpacing
            anchors.bottomMargin: _messageWrapper.defaultSpacing
            
            Component.onCompleted: {
                if (root.isLocal) {
                    _messageEncryptionStatus.anchors.left = _messageWrapper.left;
                    _messageEncryptionStatus.anchors.leftMargin = 10;
                } else {
                    _messageEncryptionStatus.anchors.right = _messageWrapper.right;
                    _messageEncryptionStatus.anchors.rightMargin = 10;
                }
            }
            
            border {
                width: 1
                color: "#626262"
            }
            
            color: (root.messageEncryptedStatus ? "green" : "red")
        }
    }
}
