import QtQuick
import QtQuick.Controls 2.15

Item {
    id: server
    property string uuid: "This is a string"
    
    Frame {
        id: channelList
        x: 0
        y: 0
        width: 163
        height: 480
        
        Rectangle {
            id: rectangle1
            x: 1
            y: 0
            width: 162
            height: 480
            color: "#ccbebe"
        }
        
        ListView {
            id: listView1
            x: 2
            y: 0
            width: 162
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
    
    Loader {
        id: channelLoader
        x: 163
        y: 0
        width: 423
        height: 480
        
        ChannelStructure {
            id: channel
            x: 0
            y: 0
        }
    }
}
