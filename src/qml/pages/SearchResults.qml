pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vermilion
import "../components"

BasePage {
  id: searchResults

  pageUrl: Qt.resolvedUrl("SearchResults.qml")

  Component.onCompleted: {
    searchBar.query = searchResults.pageArgs.searchQuery;
  }

  Connections {
    function onSearchSuccess(model) {
      resultsView.model = model;
      searchResults.pageArgs.model = model;
    }

    target: YT
  }
  ColumnLayout {
    anchors.centerIn: parent
    anchors.fill: parent

    SearchBar {
      id: searchBar

      Layout.fillWidth: true
      Layout.preferredHeight: 48
      leftPadding: height

      onAccepted: {
        YT.requestSearch(searchBar.text);
        searchResults.pageArgs.searchQuery = searchBar.text;
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
      id: resultsView

      Layout.fillHeight: true
      Layout.fillWidth: true
      clip: true
      model: searchResults.pageArgs.model

      delegate: SongItem {
        id: songItem

        height: 80
        width: ListView.view ? ListView.view.width : parent.width

        // TODO: Add working items to this.
        rightContent: Item {
          id: contextMenuDots

          Layout.alignment: Qt.AlignVCenter
          Layout.preferredHeight: 48
          Layout.preferredWidth: 40

          Text {
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 20
            font.weight: Font.Bold
            text: "⋮"
          }
          TapHandler {
            target: parent

            onSingleTapped: {
              console.log("Context Menu open requested.");
            }
          }
        }

        TapHandler {
          target: parent

          onSingleTapped: {
            resultsView.model.playTrackAtIndex(songItem.index);
          }
        }
      }
    }
  }
}
