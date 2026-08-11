import QtQuick
import QtQuick.Controls

Item {
    id: newServerMenu
    width: 263
    height: 214
    MouseArea {
        id: mouseArea123
        anchors.fill: parent
        drag.target: newServerMenu
    }
    
    Rectangle {
        id: rectangle4353
        color: "#5f5f5f"
        border.color: "#1c1c1c"
        border.width: 3
        anchors.fill: parent
    }
    
    Text {
        id: text1
        x: 8
        width: 78
        height: 21
        color: "#ffffff"
        text: "Server Name:"
        anchors.top: parent.top
        anchors.topMargin: 9
        font.pixelSize: 12
    }
    
    Text {
        id: text2
        x: 8
        width: 78
        height: 16
        color: "#ffffff"
        text: "Icon Path:"
        anchors.top: parent.top
        anchors.topMargin: 81
        font.pixelSize: 12
    }
    
    TextField {
        id: serverIconPath
        x: 8
        width: 247
        height: 20
        anchors.top: parent.top
        anchors.topMargin: 103
        font.pixelSize: 12
        placeholderText: "Icon Path"
    }
    
    Button {
        id: button
        x: 102
        y: 172
        width: 59
        height: 22
        text: "Create"
        anchors.top: parent.verticalCenter
        anchors.topMargin: 65
        
        Connections {
            target: button
            function onClicked() {
                CppInterface.createNewServer()
                newServerMenuLoader.active = false
            }
        }
    }
    
    Button {
        id: button1
        x: 224
        y: 0
        width: 39
        height: 19
        text: "Exit"
        anchors.right: parent.right
        anchors.rightMargin: 0
        
        Connections {
            target: button1
            function onClicked() { newServerMenuLoader.active = false }
        }
    }
    
    DropArea {
        id: dropArea
        anchors.fill: parent
    }

    TextField {
        id: serverName
        x: 8
        y: 36
        width: 102
        height: 15
        color: "#ffffff"
        placeholderText: "Server Name"
    }
    
}
