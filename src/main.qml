import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import com.SocialCipher.Entities 1.0

import "qrc:/views"
import "qrc:/sliders"

Window {
    id: root
    
    width: 360
    height: 640
    
    visible: true
    title: qsTr("Social Cipher")
    
    property bool isLoading: false
    
    Component.onCompleted: {
        // mainPagesStack start page setting...
        
        mainPagesStack.push(loginView);
    }
    
    onClosing: {
        appExecManager.processQuitRequest();
    }
    
    StackView {
        id: mainPagesStack
        
        anchors.fill: parent
    }
    
    function pushViewOnStack(view) {
        if (mainPagesStack.depth > 0) mainPagesStack.pop();
        
        mainPagesStack.push(view);
    }
    
    Connections {
        target: appExecManager
        
        function onErrorOccured(err) {
            errorSlider.processError(err);
        }
        
        function onOpenLoginView() {
            pushViewOnStack(loginView);
        }
        
        function onOpenDialogsView() {
            pushViewOnStack(dialogsView);
        }
        
        function onOpenDialogMessagesView() {
            pushViewOnStack(dialogMessagesView);
        }
        
        function onLoadingStarted() {
            console.log("Loading started");
            isLoading = true;
        }
        
        function onLoadingEnded() {
            console.log("Loading ended");
            isLoading = false;
        }
    }
    
    // views:
    
    LoginView {
        id: loginView
        
        //model: loginModel
        visible: false
    }
    
    DialogsView {
        id: dialogsView
        
        //model: dialogsModel
        visible: false
    }
    
    DialogMessagesView {
        id: dialogMessagesView
        
        //model: dialogMessagesModel
        visible: false
    }
    
    // header????
    
    // sliders:
    
    ErrorSlider {
        id: errorSlider
    }
    
    // loading screen:
    
    Rectangle {
        id: _busyIndicatorWrapper
        
        anchors.fill: parent
        
        visible: isLoading
        color: Qt.rgba(163, 163, 163, 0.5)
        
        BusyIndicator {
            id: busyIndicator
            
            anchors.centerIn: _busyIndicatorWrapper
            
            running: isLoading
        }
    }
}
