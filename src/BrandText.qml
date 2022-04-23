import QtQuick 2.15

Text {
    font {
        pointSize: 24
        family: _fontLoader.name
    }
    
    FontLoader { 
        id: _fontLoader
        source: "qrc:/attachments/Rancho-Regular.ttf" 
    }
}
