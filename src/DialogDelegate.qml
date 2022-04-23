import QtQuick 2.15
import QtQuick.Layouts 1.12

Rectangle {
    id: root
    
    height: 40
    
    signal dialogClicked(int peerId)
    
    property string lastMessageText: ""
    property string peerName:        ""
    property string lastMessageDate: ""
    property int    peerId:          0
    
    border.width: 1
    border.color: "darkgrey"
        
    RowLayout {
        id: _headerRow
        width: parent.width
        
        Text {
            id: _peerName
            
            Layout.fillWidth: true
            
            text: peerName
        }
        
        Text {
            id: _lastMessageDate
            
            visible: (root.width < _peerName.contentWidth + _lastMessageDate.contentWidth ? false : true)
            text: lastMessageDate
        }
    }
    
    Text {
        id: _lastMessageText
        
        anchors.bottom: parent.bottom
        width: parent.width
        
        maximumLineCount: 1
        elide: Text.ElideRight
        text: lastMessageText
    }
    
    MouseArea {
        id: _mouseArea
        
        anchors.fill: parent
        
        hoverEnabled: true
        
        onClicked: {
            dialogClicked(peerId);
        }
    }
}
