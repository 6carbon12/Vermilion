pragma ComponentBehavior: Bound

import QtQuick
import "../components"

BasePage {
  id: homePage

  pageArgs: ({})
  pageUrl: Qt.resolvedUrl("Home.qml")

  background: Rectangle {
    color: "#000000"
  }

  Column {
    anchors.centerIn: parent
    spacing: 20

    Text {
      color: "white"
      font.pixelSize: 24
      text: "Home"
    }
  }
}
