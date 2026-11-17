import QtQuick
import QtQuick.Controls 2.15

Item {
    id: folder
    width: parent.width
    height: parent.hight
    property string path: parent.path
    property string name: parent.name
    Rectangle {
        id: rectangle
        color: "#cdb7b7"
        anchors.fill: parent
    }

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
        text: name
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: image1.right
        anchors.leftMargin: 6
        font.pixelSize: 12
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        Connections {
            target: mouseArea
            function onDoubleClicked() {
                listView.model.clear()
                CppInterface.fillFileContainer(server.uuid, path)
            }
        }
    }

}
