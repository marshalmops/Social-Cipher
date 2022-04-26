import QtQuick 2.15
import QtQuick.Layouts 1.12

Rectangle {
    id: root
    
    height: 44
    
    signal dialogClicked(int peerId)
    
    property string lastMessageText: ""
    property string peerName:        ""
    property string lastMessageDate: ""
    property int    peerId:          0
    
    border.width: 1
    border.color: "darkgrey"
        
    ColumnLayout {
        width: parent.width
        
        RowLayout {
            id: _headerRow
            //width: parent.width
            Layout.fillWidth: true
            Layout.topMargin: 4
            
            Text {
                id: _peerName
                
                Layout.leftMargin: 5
                Layout.fillWidth: true
                
                text: peerName
            }
            
            Text {
                id: _lastMessageDate
                
                Layout.rightMargin: 5
                
                visible: (root.width < _peerName.contentWidth + _lastMessageDate.contentWidth ? false : true)
                text: lastMessageDate
            }
        }
        
        Text {
            id: _lastMessageText
            
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: 5
            //anchors.bottom: parent.bottom
            //width: parent.width
            
            maximumLineCount: 1
            elide: Text.ElideRight
            text: lastMessageText
        }
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
