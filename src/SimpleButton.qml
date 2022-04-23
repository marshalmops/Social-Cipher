import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: _button
    
    height: 25
    
    contentItem: Text {
        verticalAlignment:   Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        
        text: _button.text
        color: "white"
        font {
            pointSize: 14
        }
    }
    
    background: Rectangle { 
        id: _background
        
        anchors.fill: parent
        radius: 10
        
        gradient: basicBackground
    }
    
    hoverEnabled: false
    
    onPressed: {
        _background.gradient = pressedBackground
    }
    
    onReleased: {
        _background.gradient = basicBackground
    }
    
    // gradients:
    
    Gradient {
        id: basicBackground
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: "#5EF4BF"}
        GradientStop { position: 1.0; color: "#1FDE9A"}
    }
    
    Gradient {
        id: pressedBackground
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: "#1FDE9A"}
        GradientStop { position: 1.0; color: "#5EF4BF"}
    }
}
