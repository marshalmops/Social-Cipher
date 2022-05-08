import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

Rectangle {
    id: _root
    
    height: 30
    
    property string attachmentName: ""
    
    default property int attachmentIndex: -1
    
    signal attachmentCanceled(int attachmentIndex);
    
    RowLayout {
        anchors.fill: parent
        
        Text {
            id: _attachmentNameText
            
            Layout.fillWidth: true
            maximumLineCount: 1
            elide: Text.ElideRight
            
            text: _root.attachmentName
        }
        
        Button {
            id: _attachmentCancelButton
            
            width: 20
            height: width
            
            contentItem: Text {
                verticalAlignment:   Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                
                text: "X"
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
            
            onClicked: {
                attachmentCanceled(attachmentIndex);
            }
        }
    }
}
