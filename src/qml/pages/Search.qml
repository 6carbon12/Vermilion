pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Page {
  id: searchPage

  property var pageArgs: ({})
  property string pageUrl: Qt.resolvedUrl("Search.qml")

  background: Rectangle {
    color: "#000000"
  }

  Column {
    anchors.centerIn: parent
    spacing: 20

    Text {
      color: "white"
      font.pixelSize: 24
      text: "Search"
    }
  }
}
