import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts


Window {
    id: mainWindow
    width: 740
    height: 580
    visible: true
    modality: Qt.ApplicationModal
    title: qsTr("Project Fern")


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

    Loader {
        id: serverLoader
        anchors.left: serverList.right
        anchors.right: parent.right
        anchors.top: toolBar.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        source: "QML/ServerStructure.qml"
        active: false

        Rectangle {
            id: rectangle8
            color: "#3d3d3d"
            anchors.fill: parent
        }
    }

    ToolBar {
        id: toolBar
        x: 0
        y: 0
        width: 640
        height: 23

        ToolSeparator {
            id: toolSeparator
            x: 112
            y: 0
            width: 13
            height: 13

            ToolButton {
                id: toolButton
                x: -108
                y: 0
                width: 115
                height: 18
                text: "Create Server"

                Connections {
                    target: toolButton
                    function onClicked() { CppInterface.createNewServer() }
                }
            }

            ToolButton {
                id: toolButton1
                x: 6
                y: 0
                width: 103
                height: 18
                text: " Create Channel"

                Connections {
                    target: toolButton1
                    function onClicked() { CppInterface.createNewTextChannel(serverLoader.item.uuid) }
                }
            }

            ToolButton {
                id: toolButton2
                x: 109
                y: 2
                width: 103
                height: 18
                text: "Join Server"
                Connections {
                    target: toolButton2
                    function onClicked() { addServerMenu.visible = true }
                }
            }

            ToolButton {
                id: toolButton3
                x: 218
                y: 2
                width: 103
                height: 18
                text: "Create Invitation"
                Connections {
                    target: toolButton3
                    function onClicked() { CppInterface.createServerInvitation(serverLoader.item.uuid) }
                }
            }
        }
    }

    Item {
        id: serverList
        width: 55
        anchors.left: parent.left
        anchors.top: toolBar.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0

        Rectangle {
            id: rectangle
            color: "#717171"
            border.width: 0
            anchors.fill: parent
        }

        GridView {
            id: gridView
            anchors.fill: parent
            anchors.rightMargin: 0
            model: ListModel {
            }
            delegate: Item {
                id: serverSelection
                x: 5
                height: 50
                property string serverID: serverid
                Column {
                    spacing: 5
                    Rectangle {
                        width: 40
                        height: 40
                        color: colorCode
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        x: 5
                        text: name
                        font.bold: true
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }

                Button {
                    id: button
                    x: 0
                    y: 0
                    width: 44
                    height: 41
                    text: qsTr("Button")

                    Connections {
                        target: button
                        function onClicked() { console.log(serverSelection.serverID) }
                    }

                    Connections {
                        id: connections
                        target: button
                        function onClicked() {
                            // if its the first load
                            if (serverLoader.active === false) {
                                serverLoader.active = true
                                serverLoader.item.uuid = serverSelection.serverID
                                CppInterface.requestServerInfo(serverSelection.serverID)
                            }
                            else {
                                // Check if the server selected is diffrent from the current server if so change it
                                if (serverLoader.item.uuid !== serverSelection.serverID) {
                                    serverLoader.active = false
                                    serverLoader.active = true
                                    serverLoader.item.uuid = serverSelection.serverID
                                    CppInterface.requestServerInfo(serverSelection.serverID)
                                }
                            }
                        }
                    }
                }
            }
            cellWidth: 70
            cellHeight: 70

            Connections {
                target: CppInterface
                function onServerLoad(server_id) {
                    console.log(server_id)
                    gridView.model.append({name: "test", colorCode: "red", serverid: server_id})
                }
            }
        }
    }

}





/*##^##
Designer {
    D{i:0}D{i:1;invisible:true}D{i:12;locked:true}D{i:16;locked:true}D{i:17;locked:true}
D{i:24;locked:true}D{i:27}D{i:28}D{i:39}D{i:40;locked:true}D{i:41;locked:true}
}
##^##*/
