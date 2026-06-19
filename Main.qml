import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts


Window {
    id: mainWindow
    width: 640
    height: 480
    visible: true
    modality: Qt.ApplicationModal
    title: qsTr("Project Fern")

    Frame {
        id: serverList
        x: 0
        y: 0
        width: 54
        height: 480

        Rectangle {
            id: rectangle
            x: 0
            y: 0
            width: 52
            height: 480
            color: "#e0c8c8"
        }

        GridView {
            id: gridView
            x: 0
            y: 5
            width: 52
            height: 475
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
        y: 0
        width: 588
        height: 480
        source: "ServerStructure.qml"
        active: false
    }
}


