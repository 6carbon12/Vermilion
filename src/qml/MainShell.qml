pragma ComponentBehavior: Bound
import QtQml

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Vermilion

Item {
  id: rootContent

  property int activeTabIndex: 0
  property var stack: stackRepeater.itemAt(activeTabIndex)

  Component.onCompleted: {
    let history = HotReloader.savedStackHistory;
    let openPlayer = false;

    if (history && history.length > 0) {
      for (let i = 0; i < stackRepeater.count; i++) {
        let sv = stackRepeater.itemAt(i) as StackView;
        if (sv)
          sv.clear(StackView.Immediate);
      }

      for (let i = 0; i < history.length; i++) {
        if (history[i].isPlayerPageActive !== undefined) {
          if (history[i].isPlayerPageActive) {
            openPlayer = true;
          }
          continue;
        }
        if (history[i].currentIndex !== undefined) {
          rootContent.activeTabIndex = history[i].currentIndex;
          continue;
        }

        let sv = stackRepeater.itemAt(history[i].index) as StackView;
        if (sv) {
          sv.pushItem(history[i].url, {
            pageArgs: history[i].args
          } || {}, StackView.Immediate);
        }
      }

      HotReloader.savedStackHistory = [];
      console.log("History restored.");
    }
  }
  onActiveTabIndexChanged: {
    if (navigationLoader.item && "currentIndex" in navigationLoader.item) {
      let navBar = navigationLoader.item;
      if (navBar.currentIndex !== activeTabIndex) {
        navBar.currentIndex = activeTabIndex;
      }
    }
  }

  Connections {
    function onCurrentIndexChanged() {
      if (navigationLoader.item && "currentIndex" in navigationLoader.item) {
        let navBar = navigationLoader.item;
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
      Layout.preferredWidth: layoutState.state === "desktop" ? 72 : -1
      Layout.row: layoutState.state === "mobile" ? 1 : 0
      source: layoutState.state === "mobile" ? "widgets/BottomBar.qml" : "widgets/SideBar.qml"

      Behavior on Layout.preferredHeight {
        NumberAnimation {
          duration: 300
          easing.type: Easing.InOutQuad
        }
      }
    }
    Item {
      id: contentContainer

      Layout.alignment: Qt.AlignHCenter
      Layout.column: layoutState.state === "mobile" ? 0 : 1
      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.maximumWidth: 1920
      Layout.row: 0

      SwipeView {
        id: mainContentLayout

        anchors.fill: parent
        clip: true
        currentIndex: rootContent.activeTabIndex
        interactive: false

        Component.onCompleted: {
          let list = contentItem as ListView;

          list.highlightMoveDuration = 250;
          list.highlightMoveVelocity = -1;
        }

        Repeater {
          id: stackRepeater

          Layout.maximumWidth: 1920
          model: ["pages/Home.qml", "pages/Search.qml", "pages/Library.qml", "pages/Settings.qml"]

          StackView {
            id: page

            required property string modelData

            initialItem: Loader {
              anchors.fill: parent
              source: Qt.resolvedUrl(page.modelData)
            }
          }
        }
      }
      PlayerView {
        id: playerView

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        z: 10
      }
    }
  }
  Connections {
    function onCurrentIndexChanged() {
      let bottomBar = navigationLoader.item;
      if (bottomBar)
        rootContent.activeTabIndex = bottomBar.currentIndex;
    }

    target: navigationLoader.item
  }
  Connections {
    function onFileChanged() {
      console.log("File changes detected...");
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
          let pageObj = stackItem && stackItem.item ? stackItem.item : stackItem;
          if (pageObj && pageObj.pageUrl) {
            historyArray.push({
              "url": pageObj.pageUrl,
              "args": pageObj.pageArgs || {},
              "index": i
            });
          }
        }
      }

      console.log("Serialized history size:", historyArray.length);
      HotReloader.savedStackHistory = historyArray;
      HotReloader.requestReload();
    }

    target: HotReloader
  }
}
