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
        anchors.fill: parent
    }
    
    Text {
        id: text1
        x: 93
        width: 78
        height: 21
        text: "Server Name:"
        anchors.top: parent.verticalCenter
        anchors.topMargin: -78
        font.pixelSize: 12
    }
    
    TextInput {
        id: serverName
        x: 57
        width: 149
        height: 20
        anchors.top: parent.verticalCenter
        anchors.topMargin: -41
        font.pixelSize: 12
    }
    
    Text {
        id: text2
        x: 107
        y: 99
        text: "Icon Path"
        anchors.top: parent.verticalCenter
        anchors.topMargin: -8
        font.pixelSize: 12
    }
    
    TextEdit {
        id: serverIconPath
        x: 8
        y: 137
        width: 247
        height: 20
        anchors.top: parent.verticalCenter
        anchors.topMargin: 30
        font.pixelSize: 12
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
    
}
