import QtQuick 2.15
import QtQuick.Layouts 1.12

Rectangle {
    id: root
    
    height: _messageWrapper.height
    
    property string messageText:            ""
    property string messageDate:            ""
    property bool   isLocal:                false
    property bool   messageEncryptedStatus: false
    
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
        
        height: _messageEncryptionStatus.height + _messageText.contentHeight + _messageDate.contentHeight + _messageColumn.spacing * 2 + _messageText.topPadding * 2 //+ _messageColumn.padding * 3
        width: messageRectWidth
        
        border.width: 1
        border.color: "darkgrey"
        
        color: isChosen ? "lightgrey" : "white"
            
        ColumnLayout {
            id: _messageColumn
            
            width: parent.width
            spacing: 5
//            padding: 5
            
            Text {
                id: _messageText
                
                Layout.fillWidth: true
                //width: messageRectWidth
                topPadding: _messageColumn.spacing
                rightPadding: 10
                leftPadding: 10
                
                wrapMode: Text.Wrap
                text: messageText
            }
            
            Text {
                id: _messageDate
                
                //width: parent.width - _messageColumn.padding * 2
                //horizontalAlignment: (isLocal ? Text.AlignLeft : Text.AlignRight)
                Layout.alignment: (isLocal ? Qt.AlignLeft : Qt.AlignRight)
                rightPadding: 10
                leftPadding: 10
                
                text: messageDate
            }
            
            Rectangle {
                id: _messageEncryptionStatus
                
                width: 10
                height: width
                radius: width / 2
                
                Layout.alignment: (isLocal ? Qt.AlignLeft : Qt.AlignRight)
                Layout.rightMargin: (isLocal ? 0  : 10)
                Layout.leftMargin:  (isLocal ? 10 : 0)
                
                border {
                    width: 1
                    color: "#626262"
                }
                
                color: (root.messageEncryptedStatus ? "green" : "red")
            }
        }
    }
    
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
}
