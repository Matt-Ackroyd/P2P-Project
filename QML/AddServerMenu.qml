import QtQuick
import QtQuick.Controls

Item {
    id: addServerMenu
    x: 0
    y: 0
    width: 201
    height: 275
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        drag.target: addServerMenu
    }

    Rectangle {
        id: rectangle1
        color: "#4d4d4d"
        anchors.fill: parent
    }
    
    Text {
        id: text1
        x: 0
        y: 0
        width: 113
        height: 20
        color: "#ffffff"
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
            color: "#434343"
            border.color: "#000000"
            anchors.fill: parent
        }
        
        TextInput {
            id: ip
            x: 0
            y: 0
            color: "#ffffff"
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
            color: "#434343"
            border.color: "#000000"
            anchors.fill: parent
        }
        
        TextInput {
            id: port
            x: 0
            y: 0
            color: "#ffffff"
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
        color: "#ffffff"
        text: "Contact Port"
        font.pixelSize: 12
    }
    
    Item {
        id: invitation
        x: 0
        y: 188
        width: 201
        height: 21
        Rectangle {
            id: rectangle4
            x: 28
            y: 85
            color: "#434343"
            border.color: "#000000"
            anchors.fill: parent
        }
        
        TextInput {
            id: code
            x: 0
            y: 0
            color: "#ffffff"
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
        color: "#ffffff"
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
            color: "#434343"
            border.width: 1
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
            color: "#434343"
            border.width: 1
            anchors.fill: parent
        }
        
        Button {
            id: button2
            text: "Close"
            anchors.fill: parent
            icon.color: "#ffffff"
            
            Connections {
                target: button2
                function onClicked() { joinServerMenuLoader.active = false }
            }
        }
    }
    
    Item {
        id: idEntry
        x: 0
        y: 127
        width: 201
        height: 21
        Rectangle {
            id: rectangle7
            color: "#434343"
            border.color: "#000000"
            anchors.fill: parent
        }
        
        TextInput {
            id: userid
            color: "#ffffff"
            text: "58adb05c6196be187e44c75248057edd"
            anchors.fill: parent
            font.pixelSize: 12
        }
    }
    
    Text {
        id: text4
        x: 0
        y: 101
        width: 113
        height: 20
        color: "#ffffff"
        text: "userID"
        font.pixelSize: 12
    }

}
