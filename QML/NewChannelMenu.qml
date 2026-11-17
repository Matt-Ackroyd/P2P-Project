import QtQuick
import QtQuick.Controls

Item {
    id: newChannelMenu
    width: 263
    height: 214
    MouseArea {
        id: mouseArea2
        anchors.fill: parent
        drag.target: newChannelMenu
    }
    
    Rectangle {
        id: rectangle20
        color: "#5f5f5f"
        border.color: "#2a2a2a"
        border.width: 3
        anchors.fill: parent
        
        TextField {
            id: textField
            x: 8
            y: 32
            width: 67
            height: 21
            color: "#ffffff"
            placeholderText: qsTr("Text Field")
        }
    }
    
    Text {
        x: 8
        width: 78
        height: 21
        color: "#ffffff"
        text: "Channel Name:"
        anchors.top: parent.verticalCenter
        anchors.topMargin: -99
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
                CppInterface.createNewTextChannel(server.uuid)
                newServerMenuLoader.active = false
            }
        }
    }
    
    Button {
        id: button2
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
    
    
}
