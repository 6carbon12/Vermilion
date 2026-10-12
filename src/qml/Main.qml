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
}
