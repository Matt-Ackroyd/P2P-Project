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
        anchors.fill: parent
        
        TextField {
            id: textField
            x: 91
            y: 107
            width: 99
            height: 34
            placeholderText: qsTr("Text Field")
        }
    }
    
    Text {
        id: text1213
        x: 93
        width: 78
        height: 21
        text: "Channel Name:"
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
        id: button11232
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
