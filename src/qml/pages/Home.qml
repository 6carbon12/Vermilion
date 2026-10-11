pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Page {
  id: homePage

  // Serialization Interface
  property string pageUrl: "pages/Home.qml"
  property var pageArgs: ({})

  background: Rectangle { color: "#1e1e2e" }

  Column {
    anchors.centerIn: parent
    spacing: 20

    Text {
      text: "HOME"
      font.pixelSize: 24
      color: "white"
    }

    Button {
      text: "Yet to be implemented."
      onClicked: {
        console.log("[Navigation] Yet to be implemented.")
      }
    }
  }
}
