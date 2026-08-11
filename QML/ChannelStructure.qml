import QtQuick
import QtQuick.Controls 2.15

Item {
    id: channel
    property string uuid: "This is NOT string"
    width: 423
    height: 480

    Component.onCompleted: channelSplitView.restoreState(settings.channelSplitView)
    Component.onDestruction: settings.channelSplitView = channelSplitView.saveState()

    SplitView {
        id: channelSplitView
        anchors.fill: parent
        orientation: Qt.Horizontal
        handle: Rectangle {
            id: handleDelegate
            implicitWidth: 1
            implicitHeight: 1
            color: "#1F1F1F"

            containmentMask: Item {
                x: (handleDelegate.width - width) / 2
                width: 15
                height: splitView.height
            }
        }


        Item {
            id: chatBox
            anchors.left: parent.left
            anchors.right: memberList.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.leftMargin: 0
            anchors.rightMargin: 0
            anchors.topMargin: 0
            anchors.bottomMargin: 0

            SplitView.preferredWidth: 307


            Item {
                id: channelHistory
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: messageInput.top
                anchors.leftMargin: 0
                anchors.rightMargin: 0
                anchors.topMargin: 0
                anchors.bottomMargin: 0

                ListView {
                    id: messsageHistoryList
                    anchors.fill: parent
                    model: ListModel {}
                    delegate: Item {
                        id: messageContainer
                        width: parent ? parent.width : 0
                        height: messageContents.contentHeight+17+10

                        property string uuid: message_id

                        Text {
                            width: 47
                            height: 16
                            color: "#ffffff"
                            text: name
                            anchors.left: image.right
                            anchors.top: parent.top
                            anchors.leftMargin: 6
                            anchors.topMargin: 0
                        }

                        Text {
                            id: messageContents
                            height: 16
                            color: "#ffffff"
                            text: message
                            anchors.left: image.right
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.leftMargin: 6
                            anchors.rightMargin: 0
                            anchors.topMargin: 17
                            wrapMode: Text.WrapAnywhere
                        }

                        Image {
                            id: image
                            x: 0
                            y: 0
                            width: 33
                            height: 33
                            source: "qrc:/qtquickplugin/images/template_image.png"
                            fillMode: Image.PreserveAspectFit
                        }
                    }


                    Connections {
                        target: CppInterface
                        function onMessageLoad(fchannel_id, fmessage, fname, fdate) {
                            messsageHistoryList.model.append({message_id: "IDK", name: fname, message: fmessage})
                            messsageHistoryList.positionViewAtEnd()
                        }
                    }
                }
            }
            Item {
                id: messageInput
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: flickable.height


                Flickable {
                    id: flickable
                    height: Math.min(contentHeight, 150)
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    boundsBehavior: Flickable.StopAtBounds
                    flickableDirection: Flickable.VerticalFlick
                    contentWidth: width
                    contentHeight: textArea.implicitHeight

                    TextArea.flickable: TextArea {
                        id: textArea
                        color: "#000000"
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        selectedTextColor: "#ffffff"
                        selectionColor: "#5f5f5f"
                        width: parent.width
                        placeholderText: "Enter Message"


                        Keys.onReturnPressed: function(event) {
                            if (event.modifiers & Qt.ShiftModifier) {
                                event.accepted = false
                            }
                            else {
                                CppInterface.sendMessage(text, server, channel)
                                textArea.clear()
                            }
                        }
                        Connections {
                            target: textArea
                            function onImplicitHeightChanged3() { messsageHistoryList.positionViewAtEnd() }
                        }



                    }
                    ScrollBar.vertical: ScrollBar {}

                }



            }
        }
        Item {
            id: memberList
            SplitView.preferredWidth: 100
            SplitView.maximumWidth: 200
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.rightMargin: 0
            anchors.topMargin: 0
            anchors.bottomMargin: 0

            width: 100

            ListView {
                id: listView
                anchors.fill: parent
                model: ListModel {}

                delegate: Item {
                    id: member
                    height: 33
                    width: parent ? parent.width : 0
                    property string uuid: message_id

                    Text {
                        text: name
                        anchors.left: profilePic.right
                        anchors.top: parent.top
                        anchors.leftMargin: 6
                        anchors.topMargin: 4
                        font.pointSize: 14
                    }

                    Image {
                        id: profilePic
                        width: 33
                        height: 33
                        source: "qrc:/qtquickplugin/images/template_image.png"
                        fillMode: Image.PreserveAspectFit
                    }
                }
            }
        }
    }
}
