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

    Frame {
        id: serverList
        x: 0
        y: 35
        width: 54
        height: 445

        Rectangle {
            id: rectangle
            x: 0
            y: 0
            width: 52
            height: 545
            color: "#e0c8c8"
        }

        GridView {
            id: gridView
            x: 0
            y: 5
            width: 52
            height: 540
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
                    gridView.model.append({name: "test", colorCode: "red", serverid: server_id})
                }
            }
        }
    }

    Loader {
        id: serverLoader
        x: 52
        y: 35
        width: 688
        height: 545
        source: "ServerStructure.qml"
        active: false
    }

    ToolBar {
        id: toolBar
        x: 0
        y: 0
        width: 640
        height: 21

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
        }
    }
}


