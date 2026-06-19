import QtQuick
import QtQuick.Controls 2.15

Item {
    id: channel
    property string uuid: "This is NOT string"
    
    Frame {
        id: channelHistory
        x: 0
        y: 2
        width: 304
        height: 437
        
        Rectangle {
            id: rectangle2
            x: 0
            y: -2
            width: 304
            height: 439
            color: "#7a6f6f"
        }
        
        ListView {
            id: listView
            x: 0
            y: 3
            width: 304
            height: 434
            model: ListModel {
                id: messsageHistoryList
                ListElement {
                    name: "Red"
                    colorCode: "red"
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
        x: 0
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
                function onAccepted() {
                    CppInterface.sendMessage(textInput.text, server, channel)
                }
            }
            
            Connections {
                target: textInput
                function onAccepted() { messsageHistoryList.append({name: "Red"}) }
            }
        }
        
    }
}
