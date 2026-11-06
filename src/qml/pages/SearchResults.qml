pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vermilion
import "../components"
import "../widgets"

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
        anchors.verticalCenter: parent.verticalCenter
        hasState: false
        height: parent.height
        size: Qt.size(32, 32)
        source: "qrc:/icons/left-arrow.svg"
        width: parent.height

        background: Rectangle {
          color: palette.alternateBase
          radius: parent.height / 2
        }

        onClicked: {
          rootContent.stack.pop();
        }
      }
    }
    SongListView {
      id: resultsView

      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.topMargin: 12
      clip: true
      model: searchResults.pageArgs.model
    }
  }
}
