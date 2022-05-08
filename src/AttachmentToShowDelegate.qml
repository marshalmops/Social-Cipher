import QtQuick 2.15

Rectangle {
    id: _root
    
    height: width
    
    color: Qt.rgba(0, 0, 0, 0)
    
    readonly property int imageMetaType: 4102
    readonly property int docMetaType:   65537
    
    default property var attachmentLocalFileLink
    
    Component.onCompleted: {
        // creating element to show attachmen content...
        
        var component;
        var props;
        
        var attachmentLocalFileUrl =  "file:///" + attachmentLocalFileLink.localFileLink;
        
        switch (attachmentLocalFileLink.metaType) {
            case _root.imageMetaType: {
                // creating Image:
                
                component = Qt.createComponent("AttachmentImage.qml");
                props = {"imageSource": attachmentLocalFileUrl};
                
                break;
            }
            case _root.docMetaType: {
                // creating Text with clickable link:
                
                component = Qt.createComponent("AttachmentText.qml");
                props = {"link": attachmentLocalFileUrl, "previewText": qsTr("Document is available here:")};
                
                break;
            }
            default: {
                // creating Text:
                
                component = Qt.createComponent("AttachmentText.qml");
                props = {/*"link": attachmentLocalFileUrl,*/ "previewText": qsTr("Unable to recognize the attachment type!")};
            }
        }
        
        var createdObject;
        
        if (component.status == Component.Ready) {
            if ((createdObject = component.createObject(_root, props)) == null)
                console.log("Attachment shower creation error!");
        }
    }
}
