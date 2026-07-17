import QtQuick
import QtQuick.Controls 2.15

Item {
    id: server
    property string uuid: "This is a string"
    

    Frame {
        id: channelList
        x: 0
        y: 0
        width: 163
        height: 480
        


        Rectangle {
            id: rectangle1
            x: 1
            y: 0
            width: 162
            height: 480
            color: "#ccbebe"

            Button {
                id: filesChannelButton
                x: 0
                y: 0
                width: 162
                height: 19
                text: "FILES"

                Connections {
                    target: filesChannelButton
                    function onClicked() {
                        channelLoader.setSource("FileStructure.qml")
                        channelLoader.active = false
                        channelLoader.active = true
                        CppInterface.fillFileContainer(uuid, "/")
                    }
                }
            }
        }

        ListView {
            id: listView
            x: 0
            y: 24
            width: 160
            height: 456
            model: ListModel {
            }
            delegate: Row {
                spacing: 5

                Item {
                    id: channelSelection
                    width: 100
                    height: 20
                    property string channelID: channelid

                    Button {
                        id: button
                        x: 0
                        y: 0
                        width: 100
                        height: 16

                        Connections {
                            id: connections
                            target: button
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

            Connections {
                target: CppInterface
                function onChannelLoad(channel_id) { listView.model.append({name: "test", colorCode: "yellow", channelid: channel_id}) }
            }
        }
    }
    
    Loader {
        id: channelLoader
        x: 163
        y: 0
        width: 423
        height: 480
        source: "ChannelStructure.qml"
        active: false
    }
}
