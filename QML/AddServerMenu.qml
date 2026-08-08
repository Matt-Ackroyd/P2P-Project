import QtQuick
import QtQuick.Controls

Item {
    id: addServerMenu
    x: 262
    y: 110
    width: 201
    height: 275
    
    Rectangle {
        id: rectangle1
        color: "#decfcf"
        anchors.fill: parent
    }
    
    Text {
        id: text1
        x: 0
        y: 0
        width: 113
        height: 20
        text: "Contact Adress"
        font.pixelSize: 12
    }
    
    Item {
        id: contactAdress
        x: 0
        y: 21
        width: 113
        height: 21
        
        Rectangle {
            id: rectangle2
            x: 28
            y: 85
            color: "#a9a9a9"
            border.color: "#a69d9d"
            anchors.fill: parent
        }
        
        TextInput {
            id: ip
            x: 0
            y: 0
            text: "192.168.0.17"
            anchors.fill: parent
            font.pixelSize: 12
        }
    }
    
    Item {
        id: contactPort
        x: 0
        y: 73
        width: 113
        height: 21
        Rectangle {
            id: rectangle3
            x: 28
            y: 85
            color: "#a9a9a9"
            border.color: "#a69d9d"
            anchors.fill: parent
        }
        
        TextInput {
            id: port
            x: 0
            y: 0
            text: "15777"
            anchors.fill: parent
            font.pixelSize: 12
        }
    }
    
    Text {
        id: text2
        x: 0
        y: 48
        width: 113
        height: 20
        text: "Contact Port"
        font.pixelSize: 12
    }
    
    Item {
        id: invitation
        x: 0
        y: 188
        width: 113
        height: 21
        Rectangle {
            id: rectangle4
            x: 28
            y: 85
            color: "#a9a9a9"
            border.color: "#a69d9d"
            anchors.fill: parent
        }
        
        TextInput {
            id: code
            x: 0
            y: 0
            text: "48ca38ec6f1bc018a17f9369a44c7881"
            anchors.fill: parent
            anchors.leftMargin: -1
            anchors.rightMargin: 1
            anchors.topMargin: 0
            anchors.bottomMargin: 0
            font.pixelSize: 12
        }
    }
    
    Text {
        id: text3
        x: 0
        y: 162
        width: 113
        height: 20
        text: "Invitation"
        font.pixelSize: 12
    }
    
    Item {
        id: item1
        x: 1
        y: 235
        width: 200
        height: 40
        
        
        Rectangle {
            id: rectangle5
            x: 46
            y: 155
            color: "#e48989"
            anchors.fill: parent
        }
        Button {
            id: button1
            text: "Join"
            anchors.fill: parent
            
            Connections {
                target: button1
                function onClicked() { CppInterface.joinServer(ip.text, port.text, userid.text, code.text) }
            }
        }
    }
    
    Item {
        id: item2
        x: 160
        y: 0
        width: 40
        height: 42
        
        Rectangle {
            id: rectangle6
            color: "#9c9393"
            anchors.fill: parent
        }
        
        Button {
            id: button2
            text: "Close"
            anchors.fill: parent
            
            Connections {
                target: button2
                function onClicked() { addServerMenu.visible = false }
            }
        }
    }
    
    Item {
        id: idEntry
        x: 0
        y: 127
        width: 113
        height: 21
        Rectangle {
            id: rectangle7
            x: 28
            y: 85
            color: "#a9a9a9"
            border.color: "#a69d9d"
            anchors.fill: parent
        }
        
        TextInput {
            id: userid
            x: 0
            y: 0
            text: "58adb05c6196be187e44c75248057edd"
            anchors.fill: parent
            anchors.leftMargin: -1
            anchors.rightMargin: 1
            anchors.topMargin: 0
            anchors.bottomMargin: 0
            font.pixelSize: 12
        }
    }
    
    Text {
        id: text4
        x: 0
        y: 101
        width: 113
        height: 20
        text: "userID"
        font.pixelSize: 12
    }
}
