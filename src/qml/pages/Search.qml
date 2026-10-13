pragma ComponentBehavior: Bound

import QtQuick
import "../components"

BasePage {
  id: searchPage

  pageArgs: ({})
  pageUrl: Qt.resolvedUrl("Search.qml")

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
