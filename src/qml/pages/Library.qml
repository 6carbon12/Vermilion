pragma ComponentBehavior: Bound

import QtQuick
import "../components"

BasePage {
  id: libraryPage

  pageArgs: ({})
  pageUrl: Qt.resolvedUrl("Library.qml")

  Column {
    anchors.centerIn: parent
    spacing: 20

    Text {
      color: "white"
      font.pixelSize: 24
      text: "Library"
    }
  }
}
