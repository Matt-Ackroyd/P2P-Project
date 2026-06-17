import QtQuick
import QtQuick.Controls 2.15

Window {
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    TextInput {
        id: textInput
        x: 258
        y: 358
        width: 80
        height: 20
        text: "Test"
        font.pixelSize: 12

        Connections {
            target: textInput
            function onAccepted() { textInput.clear() }
        }
    }

    Button {
        id: myButton
        x: 203
        y: 194
        width: 211
        height: 93
        text: qsTr("Button")
        icon.color: "#ac9191"
        flat: false

        Connections {
            target: myButton
            function onClicked() { window.setWidth() }
        }
    }

    Rectangle {
        id: rectangle
        x: 48
        y: 98
        width: 200
        height: 200
        color: "#ab5b5b"
    }
}
