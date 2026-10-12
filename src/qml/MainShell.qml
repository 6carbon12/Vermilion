pragma ComponentBehavior: Bound

import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Vermilion

Item {
  id: rootContent

  property var stack: mainContentLayout.children[navigationLoader.item.currentIndex]

  Component.onCompleted: {
    let history = HotReloader.savedStackHistory;
    if (history && history.length > 0) {
      for (let i = 0; i < mainContentLayout.children.length; i++) {
        let page = mainContentLayout.children[i];
        if (page.item) {
          page.item.clear(StackView.Immediate);
        }
      }
      for (let i = 0; i < history.length; i++) {
        mainContentLayout.pushItem(history[i].url, history[i].args, StackView.Immediate);
      }
      HotReloader.savedStackHistory = [];
    } else {
      mainContentLayout.currentIndex = 0;
    }
  }

  StateGroup {
    id: layoutState

    state: rootContent.width < 800 ? "mobile" : "desktop"

    states: [
      State {
        name: "mobile"
      },
      State {
        name: "desktop"
      }
    ]
  }
  GridLayout {
    anchors.fill: rootContent
    columnSpacing: 0
    columns: layoutState.state === "mobile" ? 1 : 2
    rowSpacing: 0
    rows: layoutState.state === "mobile" ? 2 : 1

    Loader {
      id: navigationLoader

      Layout.column: 0
      Layout.fillHeight: layoutState.state === "desktop"

      // Adjusts its own sizing constraints based on the active layout direction
      Layout.fillWidth: layoutState.state === "mobile"
      Layout.preferredHeight: layoutState.state === "mobile" ? 48 : -1
      Layout.preferredWidth: layoutState.state === "desktop" ? 48 : -1
      Layout.row: layoutState.state === "mobile" ? 1 : 0
      source: layoutState.state === "mobile" ? "widgets/BottomBar.qml" : "widgets/SideDrawer.qml"
    }
    StackLayout {
      id: mainContentLayout

      Layout.column: layoutState.state === "mobile" ? 0 : 1
      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.row: layoutState.state === "mobile" ? 0 : 0
      clip: true
      currentIndex: navigationLoader.item.currentIndex

      Repeater {
        model: ["pages/Home.qml", "pages/Search.qml", "pages/Library.qml", "pages/Settings.qml"]

        StackView {
          id: page

          required property string modelData

          Layout.fillHeight: true
          Layout.fillWidth: true

          initialItem: Loader {
            anchors.fill: parent
            source: Qt.resolvedUrl(page.modelData)
          }
        }
      }
    }
  }
  Connections {
    function onCurrentIndexChanged() {
      if (navigationLoader.item) {
        console.log("Index changed...");
        mainContentLayout.currentIndex = navigationLoader.item.currentIndex;
      }
    }

    target: navigationLoader.item
  }
  Connections {
    function onFileChanged() {
      let historyArray = [];
      for (let i = 0; i < mainContentLayout.depth; i++) {
        let page = mainContentLayout.get(i);
        if (page && page.pageUrl) {
          historyArray.push({
            "url": page.pageUrl,
            "args": page.pageArgs
          });
        }
      }
      HotReloader.savedStackHistory = historyArray;
      HotReloader.requestReload();
    }

    target: HotReloader
  }
}
