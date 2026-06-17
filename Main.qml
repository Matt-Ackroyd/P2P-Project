import QtQuick
import QtQuick.Controls 2.15
import MyFoo 1.0
import QtQuick.Window 2.15

Window {
    id: mainWindow
    width: 640
    height: 480
    visible: true
    title: qsTr("Project Fern")

    Foo { id: foo}

    Item {
        id: serverList
        x: 0
        y: 0
        width: 60
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
                x: 5
                height: 50
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
            }
            cellWidth: 70
            cellHeight: 70
        }
    }

    Item {
        id: channelList
        x: 66
        y: 0
        width: 154
        height: 480


        Rectangle {
            id: rectangle1
            x: -8
            y: 0
            width: 162
            height: 480
            color: "#ccbebe"
        }


        ListView {
            id: listView1
            x: 0
            y: 0
            width: 154
            height: 480
            model: ListModel {
                ListElement {
                    name: "Red"
                    colorCode: "red"
                }

                ListElement {
                    name: "Green"
                    colorCode: "green"
                }

                ListElement {
                    name: "Blue"
                    colorCode: "blue"
                }

                ListElement {
                    name: "White"
                    colorCode: "white"
                }
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

    Item {
        id: messageHistory
        x: 226
        y: 0
        width: 304
        height: 439


        Rectangle {
            id: rectangle2
            x: 0
            y: 0
            width: 304
            height: 439
            color: "#7a6f6f"
        }


        ListView {
            id: listView
            x: 0
            y: 5
            width: 304
            height: 434
            model: ListModel {
                ListElement {
                    name: "Red"
                    colorCode: "red"
                }

                ListElement {
                    name: "Green"
                    colorCode: "green"
                }

                ListElement {
                    name: "Blue"
                    colorCode: "blue"
                }

                ListElement {
                    name: "White"
                    colorCode: "white"
                }
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

    Item {
        id: messageInput
        x: 226
        y: 445
        width: 304
        height: 35

        Rectangle {
            id: rectangle3
            x: 0
            y: 0
            width: 304
            height: 35
            color: "#f7cfcf"
        }

        TextInput {
            id: textInput
            x: 0
            y: 0
            width: 304
            height: 35
            text: "Test"
            font.pixelSize: 12

            Connections {
                target: textInput
                function onAccepted() { textInput.clear() }
            }

            Connections {
                target: textInput
                function onAccepted() { foo.test(textInput.text) }
            }
        }

    }






}
