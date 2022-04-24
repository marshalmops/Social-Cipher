import QtQuick 2.15
import QtQuick.Layouts 1.12

Rectangle {
    id: root
    
    height: _messageWrapper.height
    
    property string messageText: ""
    property string messageDate: ""
    property bool   isLocal:     false
    
    default property int messageRectWidth
    
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
        
        height: _messageText.contentHeight + _messageDate.contentHeight + _messageColumn.spacing + _messageColumn.padding * 2
        width:  messageRectWidth
        
        border.width: 1
        border.color: "darkgrey"
            
        Column {
            id: _messageColumn
            
            width: parent.width
            spacing: 5
            padding: 5
            
            Text {
                id: _messageText
                
                width: parent.width
                rightPadding: 10
                
                wrapMode: Text.Wrap
                text: messageText
            }
            
            Text {
                id: _messageDate
                
                width: parent.width - _messageColumn.padding * 2
                horizontalAlignment: (isLocal ? Text.AlignLeft : Text.AlignRight)
                
                text: messageDate
            }
        }
    }
}
