import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    
    height: 32
    
    border {
        width: 1
        color: "#626262"
    }
    
    clip: true
    
    readonly property string inputText: _textInput.text
    property alias placeholderText: _placeholderText.text
    
    function cleanUp() {
        _textInput.text = "";
    }
    
    MouseArea {
        anchors.fill: parent
        
        hoverEnabled: true
        cursorShape: entered ? Qt.IBeamCursor : Qt.ArrowCursor
        
        TextInput {
            id: _textInput
            
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            leftPadding: 5
            
            onFocusChanged: {
                if (_textInput.focus) {
                    _placeholderText.visible = false;
                } else {
                    if (_textInput.length === 0)
                        _placeholderText.visible = true;
                }
            }
        }
        
        Text {
            id: _placeholderText
            
            anchors.fill: parent
            anchors.leftMargin: _textInput.leftPadding
            font.pointSize: _textInput.font.pointSize
            verticalAlignment: Text.AlignVCenter
            
            color: "#626262"
        }
    }
}
