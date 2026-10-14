pragma ComponentBehavior: Bound

import QtQuick
import "../components"

BasePage {
  id: searchPage

  pageArgs: ({})
  pageUrl: Qt.resolvedUrl("Search.qml")

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
