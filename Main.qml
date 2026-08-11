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
            Action { text: qsTr("Manage Invitations")
                onTriggered: invitationManagerLoader.active = !invitationManagerLoader.active
            }

            MenuSeparator { }
            Action { text: qsTr("New Server")
                onTriggered: newServerMenuLoader.active = !newServerMenuLoader.active
            }
            Action { text: qsTr("Join Server")
                onTriggered: joinServerMenuLoader.active = !joinServerMenuLoader.active
            }

        }
        Menu {
            title: qsTr("&Help")
            Action { text: qsTr("&About") }
        }
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
                            source: "Files/" + serverid + "/icon.png"
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



    Item {
        id: menus
        anchors.fill: parent

        Loader {
            id: settingsLoader
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.leftMargin: 50
            anchors.rightMargin: 50
            anchors.topMargin: 50
            anchors.bottomMargin: 50
            source: "QML/Settings.qml"
            active: false
        }

        Loader {
            id: newChannelMenuLoader
            source: "QML/NewChannelMenu.qml"
            anchors.centerIn: parent
            active: false
        }

        Loader {
            id: newServerMenuLoader
            source: "QML/NewServerMenu.qml"
            anchors.centerIn: parent
            active: false
        }

        Loader {
            id: joinServerMenuLoader
            source: "QML/AddServerMenu.qml"
            anchors.centerIn: parent
            active: false
        }

        Loader {
            id: invitationManagerLoader
            anchors.centerIn: parent
            active: false
        }
    }

}









/*##^##
Designer {
    D{i:0}D{i:19;locked:true}D{i:33}D{i:34}D{i:35}D{i:36}D{i:37}D{i:38}
}
##^##*/
