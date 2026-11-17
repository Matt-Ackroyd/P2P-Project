import QtQuick
import QtQuick.Controls 2.15

Item {
    id: server
    property string uuid: "This is a string"

    Component.onCompleted: {
        splitView.restoreState(settings.splitView)
        serverSplitView.restoreState(settings.serverSplitView)
    }
    Component.onDestruction: {
        settings.splitView = splitView.saveState()
        settings.serverSplitView = serverSplitView.saveState()
    }


    SplitView {
        id: serverSplitView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: 0
        orientation: Qt.Horizontal
        handle: Rectangle {
            id: serverHandleDelegate
            implicitWidth: 1
            implicitHeight: 1
            color: "#1F1F1F"

            containmentMask: Item {
                x: (serverHandleDelegate.width - width) / 2
                width: 20
                height: serverSplitView.height
            }
        }


        Item {
            SplitView.minimumWidth: 50
            SplitView.preferredWidth: 162
            id: channelList
            width: 162
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: 0
            anchors.bottomMargin: 0


            Rectangle {
                id: rectangle
                color: "#444444"
                anchors.fill: parent
            }

            Button {
                id: filesChannelButton
                x: 0
                width: parent ? parent.width : 0
                height: 19
                text: "FILES"
                anchors.top: parent.top
                anchors.topMargin: 0

                Connections {
                    target: filesChannelButton
                    function onClicked() {
                        channelLoader.setSource("FileStructure.qml")
                        channelLoader.active = false
                        channelLoader.active = true
                        CppInterface.fillFileContainer(uuid, "Files/" + server.uuid + "/")
                    }
                }
            }
            SplitView {
                id: splitView
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: filesChannelButton.bottom
                anchors.bottom: parent.bottom
                anchors.topMargin: 0
                orientation: Qt.Vertical
                handle: Rectangle {
                    id: handleDelegate
                    implicitWidth: 1
                    implicitHeight: 1
                    color: "#1F1F1F"

                    containmentMask: Item {
                        x: (handleDelegate.width - width) / 2
                        width: 20
                        height: splitView.height
                    }
                }

                ListView {
                    id: listView
                    height: 55
                    SplitView.preferredHeight: 250
                    SplitView.minimumHeight: 50
                    model: ListModel {
                    }
                    delegate: Item {
                        id: channelSelection
                        width: parent ? parent.width : 0
                        height: 20
                        property string channelID: channelid


                        Text {
                            color: "#ffffff"
                            text: name
                            anchors.fill: parent
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent

                            Connections {
                                target: mouseArea
                                function onClicked() {
                                    channelLoader.setSource("ChannelStructure.qml")
                                    // if its the first load
                                    if (channelLoader.active === false) {
                                        channelLoader.active = true
                                        channelLoader.item.uuid = channelSelection.channelID
                                        CppInterface.requestChannelInfo(server, channelLoader.item)
                                    }
                                    else {
                                        // Check if the server selected is diffrent from the current server if so change it
                                        if (channelLoader.item.uuid !== channelSelection.channelID) {
                                            channelLoader.active = false
                                            channelLoader.active = true
                                            channelLoader.item.uuid = channelSelection.channelID
                                            CppInterface.requestChannelInfo(server, channelLoader.item)
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Connections {
                        target: CppInterface
                        function onChannelLoad(channel_id) { listView.model.append({name: "test", colorCode: "yellow", channelid: channel_id}) }
                    }
                }

                ListView {
                    id: voiceChannels
                    SplitView.minimumHeight: 50
                    model: ListModel {

                    }
                    delegate: Row {
                        spacing: 5
                        Rectangle {
                            width: 100
                            height: 20
                            color: colorCode
                        }

                        Text {
                            width: 100
                            text: name
                        }
                    }
                }

            }
        }
        Loader {
            SplitView.minimumWidth: 50
            id: channelLoader
            width: 423
            height: 480
            source: "ChannelStructure.qml"
            active: false
        }
    }





    
}


