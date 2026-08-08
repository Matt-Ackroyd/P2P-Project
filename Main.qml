import QtQuick
import QtCore
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: mainWindow
    width: 740
    height: 580
    visible: true
    color: "#3d3d3d"
    modality: Qt.ApplicationModal
    title: qsTr("Project Fern")

    Settings {
        id: settings
        property alias x: mainWindow.x
        property alias y: mainWindow.y
        property alias width: mainWindow.width
        property alias height: mainWindow.height
        property var serverSelectionSplitView
        property var serverSplitView
        property var splitView
        property var channelSplitView
    }


    Shortcut {
        sequence: "Tab"

        onActivated: {
            menuBar.visible = !menuBar.visible;

            if (menuBar.height === 0) {
                menuBar.height = 31;
            } else {
                menuBar.height = 0;
            }
        }
    }


    MenuBar {
        id: menuBar
        height: 31
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.topMargin: 0

        Menu {
            title: qsTr("Acount")
            Action { text: qsTr("Settings")
                onTriggered: settingsLoader.active = !settingsLoader.active
            }
            MenuSeparator { }
            Action {
                text: qsTr("All Known Users")
                //onTriggered:
            }
        }
        Menu {
            title: qsTr("Server")
            Action { text: qsTr("Create New Channel")
                onTriggered: newChannelMenuLoader.active = !newChannelMenuLoader.active
            }
            Action { text: qsTr("Manage Invitations") }

            MenuSeparator { }
            Action { text: qsTr("New Server")
                onTriggered: newServerMenuLoader.active = !newServerMenuLoader.active
            }
            Action { text: qsTr("Join Server")
                onTriggered: newServerMenuLoader.active = true
            }

        }
        Menu {
            title: qsTr("&Help")
            Action { text: qsTr("&About") }
        }
    }

    Loader {
        id: settingsLoader
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: menuBar.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 31
        anchors.rightMargin: 31
        anchors.topMargin: 31
        anchors.bottomMargin: 31
        source: "QML/Settings.qml"
        active: false
    }

    Component.onCompleted: serverSelectionSplitView.restoreState(settings.serverSelectionSplitView)
    Component.onDestruction: settings.serverSelectionSplitView = serverSelectionSplitView.saveState()

    SplitView {
        id: serverSelectionSplitView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: menuBar.bottom
        anchors.bottom: parent.bottom
        anchors.topMargin: 0
        orientation: Qt.Horizontal
        handle: Rectangle {
            id: handleDelegate
            implicitWidth: 1
            implicitHeight: 1
            color: "#1F1F1F"

            containmentMask: Item {
                x: (handleDelegate.width - width) / 2
                width: 20
                height: serverSelectionSplitView.height
            }
        }

        Item {
            id: serverList
            width: 55
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 0
            anchors.topMargin: 0
            SplitView.minimumWidth: 50
            //anchors.top: toolBar.bottom

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
                boundsMovement: Flickable.StopAtBounds
                model: ListModel {
                }
                delegate: Item {
                    id: serverSelection
                    x: 5
                    height: 70

                    width: 50

                    property string serverID: serverid


                    Item {
                        id: serverBoarder
                        width: 50
                        height: 50
                        anchors.verticalCenterOffset: -5
                        anchors.centerIn: parent

                        Column {
                            spacing: 5
                        }

                        Rectangle {
                            width: 40
                            height: 40
                            color: colorCode
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            x: 13
                            y: 45
                            text: name
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Image {
                            id: image
                            anchors.fill: parent
                            source: "qrc:/qtquickplugin/images/template_image.png"
                            fillMode: Image.PreserveAspectFit
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        x: 10
                        y: 5
                        anchors.fill: parent

                        Connections {
                            target: mouseArea
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

        Loader {
            id: serverLoader
            source: "QML/ServerStructure.qml"
            active: false
        }

    }



    Loader {
        id: menuLoader
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: 239
        anchors.rightMargin: 239
        anchors.topMargin: 143
        anchors.bottomMargin: 227
        active: false
        sourceComponent: newServerMenu
    }
}









/*##^##
Designer {
    D{i:0}D{i:16}D{i:20;locked:true}D{i:34}
}
##^##*/
