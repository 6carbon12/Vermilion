pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Vermilion

ApplicationWindow {
  id: root

  height: 600
  palette.accent: "#9F9FFF"
  palette.alternateBase: "#1A1A1A"
  palette.text: "#FFFFFF"
  palette.window: "#000000"
  title: "Vermilion"
  visible: true
  width: 800

  contentItem.Keys.onPressed: event => {
    if (event.key === Qt.Key_Back) {
      event.accepted = true;
      if (contentLoader.item && contentLoader.item.stack) {
        if (contentLoader.item.stack.depth <= 1) {
          Qt.quit();
        } else {
          contentLoader.item.stack.pop();
        }
      }
    }
  }

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
      console.log("Player Error:", err);
    }

    target: Player
  }
}
