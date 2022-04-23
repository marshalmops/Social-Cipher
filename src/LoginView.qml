import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/components"

ViewBase {
    id: _root
    
    Rectangle {
        width: _contentForm.implicitWidth
        height: _contentForm.implicitHeight
        
        anchors.centerIn: parent
        
        Column {
            id: _contentForm
            
            anchors.fill: parent
            
            spacing: 5
            
            BrandText {
                anchors.horizontalCenter: parent.horizontalCenter
                
                text: "Social Cipher"
            }
            
            // social networks checking row...
            
            CustomTextInput {
                id: _token
                
                width: (_root.width < 200 ? _root.width : 200)
                
                placeholderText: qsTr("Enter your token...")
            }
            
            SimpleButton {
                width: 100
                anchors.horizontalCenter: parent.horizontalCenter
                
                text: qsTr("Sign In")
                
                onClicked: {
                    if (_token.inputText.length == 0) return;
                    
                    appExecManager.startLoading();
                    
                    loginModel.signInWithToken(_token.inputText);
                }
            }
        }
    }
}
