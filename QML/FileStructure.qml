import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15


Item {
    id: item1

    Rectangle {
        id: rectangle
        color: "#eed9d9"
        anchors.fill: parent
    }

    DropArea {
        id: dropArea
        anchors.fill: parent

        Pane {
            id: pane
            x: 0
            y: 41
            width: 640
            height: 20
        }
    }

    ToolBar {
        id: toolBar
        x: 0
        y: 0
        width: 640
        height: 40

        ToolButton {
            id: toolButton
            y: 0
            text: qsTr("Tool Button")
            anchors.left: parent.left
            anchors.leftMargin: 0

            Connections {
                target: toolButton
                function onClicked() {
                    //CppInterface.fillFileContainer(column, "/")

                    //itemLoader.source = "Folder.qml";

                    listView.model.append({fileName: "File.qml"});
                    listView.model.append({fileName: "Folder.qml"});
                }
            }
        }
    }

    ListView {
        id: listView
        x: 0
        y: 60
        width: 640
        height: 420
        model: ListModel{}
        delegate: Loader {
                source: fileName
            }
        }

}


