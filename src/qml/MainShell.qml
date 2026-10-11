pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Vermilion.Dev

Item {
  id: rootContent

  StackView {
    id: mainContentStack
    anchors.fill: parent
  }

  Component.onCompleted: {
    let history = HotReloader.savedStackHistory
    if (history && history.length > 0) {
      mainContentStack.clear(StackView.Immediate)
      for (let i = 0; i < history.length; i++) {
        mainContentStack.push(history[i].url, history[i].args, StackView.Immediate)
      }
      HotReloader.savedStackHistory = []
    } else {
      mainContentStack.push("pages/Home.qml", {}, StackView.Immediate)
    }
  }

  Connections {
    target: HotReloader

    function onFileChanged() {
      let historyArray = []
      for (let i = 0; i < mainContentStack.depth; i++) {
        let page = mainContentStack.get(i)
        if (page && page.pageUrl) {
          historyArray.push({
            "url": page.pageUrl,
            "args": page.pageArgs
          })
        }
      }
      HotReloader.savedStackHistory = historyArray
      HotReloader.requestReload()
    }
  }
}
