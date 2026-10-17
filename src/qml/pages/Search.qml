pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vermilion
import "../components"

BasePage {
  id: searchPage

  pageArgs: ({})
  pageUrl: Qt.resolvedUrl("Search.qml")

  Connections {
    id: ytConnection
    enabled: searchPage.StackView.visible
    function onSearchSuccess(model) {
      rootContent.stack.pushItem(Qt.resolvedUrl("SearchResults.qml"), {
        pageArgs: {
          model: model,
          searchQuery: searchBar.query
        }
      }, StackView.Immediate);
      searchBar.query = ""
    }

    target: YT
  }
  ColumnLayout {
    anchors.fill: parent
    spacing: 8

    SearchBar {
      id: searchBar

      Layout.fillWidth: true
      Layout.preferredHeight: 48
      leftPadding: height

      onAccepted: {
        YT.requestSearch(searchBar.query);
      }

      IconButton {
        id: backButton

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        hasState: false
        height: 42
        size: Qt.size(32, 32)
        source: "../../../assets/icons/left-arrow.svg"

        background: Rectangle {
          color: palette.alternateBase
          radius: parent.height / 2
        }

        onClicked: {
          rootContent.stack.pop();
        }
      }
    }
    ListView {
      id: recentlySearchedSongs

      Layout.alignment: Qt.AlignCenter
      Layout.fillWidth: true
      Layout.preferredHeight: 100
      clip: true

      model: 16

      orientation: ListView.Horizontal
      spacing: 10

      delegate: Rectangle {
        color: palette.alternateBase
        height: recentlySearchedSongs.height
        radius: 8
        width: 100
      }
    }
    ListView {
      id: searchHistoryView
      Layout.fillHeight: true
      Layout.fillWidth: true
      clip: true

      delegate: Item {
        id: itm

        required property var query

        height: 56
        width: ListView.view.width

        TapHandler {
          onTapped: {
            searchBar.query = itm.query;
            YT.requestSearch(itm.query);
          }
        }
        Text {
          anchors.left: parent.left
          anchors.right: parent.right
          anchors.rightMargin: 16
          anchors.leftMargin: 8
          anchors.verticalCenter: parent.verticalCenter
          color: "white"
          elide: Text.ElideRight
          font.pixelSize: 16
          text: itm.query
        }
      }
      model: ListModel {
        ListElement {
          query: "Daft Punk Discovery full album"
        }
        ListElement {
          query: "Lofi hip hop beats to relax/study to"
        }
        ListElement {
          query: "Kendrick Lamar live performance"
        }
        ListElement {
          query: "Tokyo Night electronic mix"
        }
        ListElement {
          query: "Arctic Monkeys AM"
        }
        ListElement {
          query: "Hans Zimmer Live in Prague"
        }
        ListElement {
          query: "Top 50 Global Hits 2026"
        }
        ListElement {
          query: "Nirvana MTV Unplugged in New York"
        }
        ListElement {
          query: "Synthwave retrowave radio"
        }
        ListElement {
          query: "Pink Floyd The Dark Side of the Moon"
        }
        ListElement {
          query: "Jazz classics playlist"
        }
        ListElement {
          query: "Beethoven Symphony No. 9"
        }
      }
    }
  }
}
