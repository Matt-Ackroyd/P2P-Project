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
            id: messsageHistoryList
            x: 0
            y: -2
            width: 304
            height: 439
            model: ListModel {}
            delegate: Row {
                property string uuid: message_id
                spacing: 5
                
                Text {
                    text: name
                }

                Text {
                    text: message
                }
            }

            Connections {
                target: CppInterface
                function onMessageLoad(fmessage_id, fmessage) {messsageHistoryList.model.append({message_id: fmessage_id,
                                                                                                    name: "you", message: fmessage})}
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
                function onAccepted() { messsageHistoryList.model.append({name:  "You: ", message: textInput.text}) }
            }
        }
        
    }
}
