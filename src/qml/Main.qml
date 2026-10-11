pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Vermilion.Dev 

ApplicationWindow {
  id: root
  width: 800
  height: 600
  visible: true
  title: "Vermilion"

  Loader {
    id: contentLoader
    anchors.fill: parent
    source: "MainShell.qml"
  }

  Connections {
    target: HotReloader

    function onReloadUI() {
      contentLoader.source = ""
      contentLoader.source = "MainShell.qml"
    }
  }
}
