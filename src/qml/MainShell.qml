pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Vermilion
import "./widgets"

Item {
  id: rootContent

  property int activeTabIndex: 0
  property var stack: stackRepeater.itemAt(activeTabIndex)

  Component.onCompleted: {
    let history = HotReloader.savedStackHistory;
    console.log("Restoring history...");

    if (history && history.length > 0) {
      for (let i = 0; i < stackRepeater.count; i++) {
        let sv = stackRepeater.itemAt(i) as StackView;
        if (sv)
          sv.clear(StackView.Immediate);
      }

      for (let i = 0; i < history.length; i++) {
        if (history[i].currentIndex !== undefined) {
          rootContent.activeTabIndex = history[i].currentIndex;
          continue;
        }

        let sv = stackRepeater.itemAt(history[i].index) as StackView;
        if (sv) {
          sv.pushItem(history[i].url, history[i].args || {}, StackView.Immediate);
        }
      }
      HotReloader.savedStackHistory = [];
    }
  }
  onActiveTabIndexChanged: {
    if (navigationLoader.item && "currentIndex" in navigationLoader.item) {
      let navBar = navigationLoader.item as BottomBar;
      if (navBar.currentIndex !== activeTabIndex) {
        navBar.currentIndex = activeTabIndex;
      }
    }
  }

  Connections {
    function onCurrentIndexChanged() {
      if (navigationLoader.item && "currentIndex" in navigationLoader.item) {
        let navBar = navigationLoader.item as BottomBar;
        if (rootContent.activeTabIndex !== navBar.currentIndex) {
          rootContent.activeTabIndex = navBar.currentIndex;
        }
      }
    }

    target: navigationLoader.item
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
      Layout.fillWidth: layoutState.state === "mobile"
      Layout.preferredHeight: layoutState.state === "mobile" ? 68 : -1
      Layout.preferredWidth: layoutState.state === "desktop" ? 68 : -1
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

      // Bind StackLayout to the hoisted root state
      currentIndex: rootContent.activeTabIndex

      Repeater {
        id: stackRepeater

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
      let bottomBar = navigationLoader.item as BottomBar;
      if (bottomBar)
        rootContent.activeTabIndex = bottomBar.currentIndex;
    }

    target: navigationLoader.item
  }
  Connections {
    function onFileChanged() {
      console.log("MainShell: File changes detected...");
      let historyArray = [];
      historyArray.push({
        "currentIndex": rootContent.activeTabIndex
      });

      for (let i = 0; i < stackRepeater.count; i++) {
        let sv = stackRepeater.itemAt(i) as StackView;
        if (!sv)
          continue;

        for (let j = 0; j < sv.depth; j++) {
          let stackItem = sv.get(j); // Might be a loader OR the loaded component.
          let pageObj = stackItem.item ? stackItem.item : stackItem;
          if (pageObj && pageObj.pageUrl) {
            historyArray.push({
              "url": pageObj.pageUrl,
              "args": pageObj.pageArgs || {},
              "index": i
            });
          }
        }
      }

      console.log("MainShell: History Size:", historyArray.length);
      HotReloader.savedStackHistory = historyArray;
      HotReloader.requestReload();
    }

    target: HotReloader
  }
}
