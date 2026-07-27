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

        Connections {
            target: dropArea
            function onDropped(drop) {
                for (var i = 0; i < drop.urls.length; i++) {
                    var path = drop.urls[i].toString()
                    // remove prefixed "file:///"
                    path = path.replace(/^(file:\/{3})/,"");
                    // unescape html codes like '%23' for '#'
                    var cleanPath = decodeURIComponent(path);

                    CppInterface.createNewFileIndicator(cleanPath)

                }
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
            id: toolButton
            y: 0
            text: "new Folder"
            anchors.left: parent.left
            anchors.leftMargin: 0

            Connections {
                target: toolButton
                function onClicked() {
                    //CppInterface.fillFileContainer(column, "/")

                    //itemLoader.source = "Folder.qml";


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
        boundsBehavior: Flickable.StopAtBounds
        boundsMovement: Flickable.StopAtBounds
        model: ListModel{}
        delegate: Loader {
            property string path: pathString
            property string uuid: uuidString
            property string name: nameString
            width: parent.width
            height: 22
            source: fileName
        }

        Connections {
            target: CppInterface
            function onFileLoad(fname, fpath, fuuid) {
                listView.model.append({fileName: "File.qml", pathString: fpath, uuidString: fuuid, nameString: fname});
            }
        }

        Connections {
            target: CppInterface
            function onFolderLoad(fname, fpath) {
                listView.model.append({fileName: "Folder.qml", pathString: fpath, nameString: fname});
            }
        }
    }
}


