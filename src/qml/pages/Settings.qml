pragma ComponentBehavior: Bound

import QtQuick
import "../components"

BasePage {
  id: settingsPage

  pageArgs: ({})
  pageUrl: Qt.resolvedUrl("Settings.qml")

  Column {
    anchors.centerIn: parent
    spacing: 20

    Text {
      color: "white"
      font.pixelSize: 24
      text: "Settings"
    }
  }
}
