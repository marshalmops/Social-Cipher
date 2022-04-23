import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: _root
    
    width: height
    
    contentItem: Text {
        verticalAlignment:   Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        
        text: "<"
        color: "black"
        font {
            pointSize: 16
        }
    }
    
    background: Rectangle { 
        id: _background
        
        anchors.fill: parent
        
        color: Qt.rgba(0, 0, 0, 0)
    }
    
    hoverEnabled: false
}
