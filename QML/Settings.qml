import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Item {

    Rectangle {
        id: rectangle
        color: "#ffffff"
        anchors.fill: parent
    }

    ColumnLayout {
        id: columnLayout
        x: 0
        y: 0
        width: 640
        height: 480

        Item {
            id: item1
            width: 640
            height: 200

            TextField {
                id: usernameQuarry
                x: 93
                y: 21
                width: 225
                height: 20
                placeholderText: "Text Field"

                Connections {
                    target: usernameQuarry
                    function onAccepted() {
                        CppInterface.username = usernameQuarry.text
                        usernameQuarry.clear()
                    }
                }
            }

            Text {
                id: text1
                x: 30
                y: 21
                width: 57
                height: 20
                text: "Username:"
                font.pixelSize: 12
            }
        }

        Item {
            id: item2
            width: 200
            height: 200
        }

        Item {
            id: item3
            width: 200
            height: 200
        }
    }

    Button {
        id: closeSettings
        x: 565
        y: 441
        width: 67
        height: 31
        text: "Close"

        Connections {
            target: closeSettings
            function onClicked() { settingsLoader.active = false }
        }
    }


}
