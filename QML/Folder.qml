import QtQuick

Item {
    id: folder
    x: 0
    y: 127
    width: 640
    height: 22
    property string path: "This is a string"
    Image {
        id: image1
        y: 0
        width: folder.height
        height: folder.height
        anchors.left: parent.left
        source: "qrc:/qtquickplugin/images/template_image.png"
        fillMode: Image.PreserveAspectFit
    }
    
    BorderImage {
        id: borderImage1
        anchors.fill: parent
        source: "qrc:/qtquickplugin/images/template_image.png"
    }
    
    Text {
        id: fileName1
        width: 246
        height: 16
        text: "FileName"
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: image1.right
        anchors.leftMargin: 6
        font.pixelSize: 12
    }
}
