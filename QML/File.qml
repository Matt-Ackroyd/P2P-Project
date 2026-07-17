import QtQuick
import QtQuick.Controls 2.15

Item {
    id: file
    x: 0
    y: 80
    width: 640
    height: 22
    property string uuid: "This is a string"
    property string path: "This is a string"
    
    Image {
        id: image
        y: 0
        width: file.height
        height: file.height
        anchors.left: parent.left
        anchors.leftMargin: 0
        source: "qrc:/qtquickplugin/images/template_image.png"
        fillMode: Image.PreserveAspectFit
    }
    
    Image {
        id: authorPic
        x: 601
        width: 26
        height: 22
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 13
        anchors.topMargin: 0
        source: "qrc:/qtquickplugin/images/template_image.png"
        fillMode: Image.PreserveAspectFit
    }
    
    BorderImage {
        id: borderImage
        anchors.fill: parent
        source: "qrc:/qtquickplugin/images/template_image.png"
    }
    
    Text {
        id: fileName
        width: 246
        height: 16
        text: "FileName"
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: image.right
        anchors.leftMargin: 6
        font.pixelSize: 12
    }
    
    Button {
        id: button
        width: 44
        height: 20
        text: qsTr("Button")
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 88
    }
}
