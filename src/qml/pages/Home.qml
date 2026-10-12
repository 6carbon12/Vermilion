pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Page {
  id: homePage

  property var pageArgs: ({})
  property string pageUrl: Qt.resolvedUrl("Home.qml")

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
