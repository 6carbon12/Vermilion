pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Vermilion

ApplicationWindow {
  id: root

  height: 600
  title: "Vermilion"
  visible: true
  width: 800
  palette.window: "#000000"
  palette.alternateBase: "#1A1A1A"
  palette.text: "#FFFFFF"
  palette.accent: "#9F9FFF"

  Loader {
    id: contentLoader

    anchors.fill: parent
    source: "MainShell.qml"
  }
  Connections {
    function onReloadUI() {
      contentLoader.source = "";
      contentLoader.source = "MainShell.qml";
    }

    target: HotReloader
  }

  Connections {
    function onErrorOccured(err) {
      console.log("Player Error:", err)
    }
    target: Player
  }
}
