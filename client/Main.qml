import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    minimumWidth:600
    minimumHeight:300
    visible: true

    ListView
    {
        id: table
        anchors.fill: parent
        model: clientsModel
        property int itemWidth:width/4
        component TableText: Text{width:table.itemWidth; horizontalAlignment:Qt.AlignHCenter}
        header: Row
        {   
            TableText{text:"Server URL"}
            TableText{text:"Timestamp"}
            TableText{text:"Status"}
            TableText{}
        }
        delegate: Row 
        {
            TableText { text: modelData.serverUrl}
            TableText { text: modelData.timestamp}
            TableText { text: modelData.online?"online":"offline"; color: modelData.online?"green":"red"}
            Button{text: "Connect"; checkable: true; Binding on checked{value:modelData.running} onClicked: modelData.running=checked; width:table.itemWidth}
        }    
    }
}