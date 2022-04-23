import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    
    border {
        width: 1
        color: "#626262"
    }
    
    clip: true
    
    property alias inputText: _textInput.text
    property alias placeholderText: _placeholderText.text

    function cleanUp() {
        _textInput.clear();
    }
    
    signal returnPressed();
    
    Flickable {
        anchors.fill: parent
        contentHeight: _textInput.contentHeight + _textInput.font.pointSize
        
        TextEdit {
            id: _textInput
            
            anchors.fill: parent
            padding: 5
            wrapMode: TextEdit.Wrap
            
            onTextChanged: {
                _placeholderText.visible = (_textInput.length === 0);
            }
            
            Keys.onReturnPressed: {
                root.returnPressed();
            }
        }
    }
    
    Text {
        id: _placeholderText
        
        anchors.fill: parent
        anchors.leftMargin: _textInput.padding
        anchors.topMargin: _textInput.padding
        font.pointSize: _textInput.font.pointSize
        
        //verticalAlignment: Text.AlignTop
        
        color: "#626262"
    }
}
