import QtQuick 2.15

Rectangle {
    
    anchors.fill: parent
    
    color: Qt.rgba(0, 0, 0, 0)
    
    default property string imageSource
    
    Image {
        id: _image
        
        anchors.fill: parent
        
        source: imageSource
        fillMode: Image.PreserveAspectFit
        
        Component.onCompleted: {
            
        }
    }
}
