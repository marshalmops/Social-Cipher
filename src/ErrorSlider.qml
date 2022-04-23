import QtQuick 2.15

import "qrc:/components"

Rectangle {
    id: root
    
    width: parent.width
    height: parent.height
    y: defaultY
    
    readonly property int defaultY: (-1) * parent.height
    
    QtObject {
        id: _privateProps
        
        property var errorsQueue: []
        property var currentError: null
        
        function slideOut() {
            root.y = 0;
        }
        
        function slideIn() {
            root.y = root.defaultY;
        }
    }
    
    function processError(error) {
        appExecManager.endLoading();
        
        _privateProps.errorsQueue.push(error);
        
        if (_privateProps.currentError == null) {
            _privateProps.currentError = _privateProps.errorsQueue.shift();
            
            _privateProps.slideOut();
        }
    }
    
    Behavior on y {
        NumberAnimation {
            duration: 300
            easing.type: Easing.InCurve
        }
    }
    
    Column {
        anchors.centerIn: parent
        width: parent.width * 0.8

        spacing: 10
        
        TitleText {
            anchors.horizontalCenter: parent.horizontalCenter
            
            text: qsTr("Error")
        }
        
        Text {
            id: _errorText
            
            anchors.horizontalCenter: parent.horizontalCenter
            
            text: (_privateProps.currentError == null ? "" : _privateProps.currentError.text)
        }
        
        SimpleButton {
            width: 100
            anchors.horizontalCenter: parent.horizontalCenter
            
            text: "OK"
            
            onClicked: {
                if (_privateProps.currentError.isCritical) {
                    appExecManager.processQuitRequest();
                } else {
                    if (_privateProps.errorsQueue.length == 0) {
                        _privateProps.currentError = null;
                        
                        _privateProps.slideIn();
                        
                        return;
                    }
                    
                    _privateProps.currentError = _privateProps.errorsQueue.shift();
                }
            }
        }
    }
    
    Connections {
        
    }
}
