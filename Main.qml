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
                ListElement {
                    name: "Grey"
                    colorCode: "grey"
                }

                ListElement {
                    name: "Red"
                    colorCode: "red"
                }

                ListElement {
                    name: "Blue"
                    colorCode: "blue"
                }

                ListElement {
                    name: "Green"
                    colorCode: "green"
                }
            }
            delegate: Item {
                id: item1
                x: 5
                height: 50
                property string serverID: ""
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
                        function onClicked() { CppInterface.test() }
                    }

                    Connections {
                        target: button
                        function onClicked() { item1.visible = true }
                    }
                }
            }
            cellWidth: 70
            cellHeight: 70

            Connections {
                target: CppInterface
                function onServerAdd(signal_param) { console.log(signal_param) }
            }
        }
    }

    Loader {
        id: serverLoader
        x: 52
        y: 0
        width: 588
        height: 480
    }

    ServerStructure {
        id: server
        x: 52
        y: 0
        width: 585
        height: 478
    }

















}
