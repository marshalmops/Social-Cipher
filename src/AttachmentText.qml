import QtQuick 2.15
import QtQuick.Layouts 1.12

Rectangle {
    id: _root
    
    anchors.fill: parent
    
    color: Qt.rgba(0, 0, 0, 0)
    
    default property string previewText: ""
    property string link: ""
    
    Rectangle {
        width: parent.width * 0.8
        height: _previewIcon.height + _previewTextShower.contentHeight 
        anchors.centerIn: parent
        
        color: Qt.rgba(0, 0, 0, 0)
        
        Column {
            anchors.fill: parent
            
            spacing: 5
            
            Image {
                // file icon:
                
                id: _previewIcon
                
                height: 40
                width: height
                anchors.horizontalCenter: parent.horizontalCenter
                
                fillMode: Image.PreserveAspectFit
                
                source: "qrc:/attachments/docIcon.png"
            }
            
            Text {
                id: _previewTextShower
                
                width: parent.width
                height: contentHeight
                horizontalAlignment: Text.AlignHCenter
                
                font {
                    pointSize: 14
                }
                
                wrapMode: Text.Wrap
                textFormat: Text.RichText
                text: _root.previewText + (_root.link.length > 0 ? " <a href=\"" + _root.link + "\">" + qsTr("Click") + "</a>" : "")
                
                onLinkActivated: (fileLink) => Qt.openUrlExternally(fileLink);
            }
        }
    }
}
