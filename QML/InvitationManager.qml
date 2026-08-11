import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Item {
    id: inviataionManager
    ListView {
        id: listView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: toolBar.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        model: ListModel {
        }
        delegate: Item {
            id: item1

            Rectangle {
                height: 20
                color: colorCode
                border.width: 1
                anchors.left: parent.left
                anchors.right: parent.right
            }

            Text {
                y: 0
                height: 16
                text: code
                anchors.left: parent.left
                anchors.right: button.left
                anchors.leftMargin: 0
                anchors.rightMargin: 0
            }

            Button {
                id: button
                x: 614
                y: 1
                width: 19
                height: 15
                text: "-"
                anchors.right: parent.right
                anchors.rightMargin: 0
            }
        }
    }

    ToolBar {
        id: toolBar
        x: 0
        y: 0
        width: 640
        height: 40

        ToolButton {
            text: "New Invitation"
            onClicked: {}
        }

    }

}
