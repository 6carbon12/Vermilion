pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
 import QtQuick.Controls
import Vermilion
import "../components"
import "../widgets"

BasePage {
  id: searchResults

  property bool isLoading: true

  pageUrl: Qt.resolvedUrl("SearchResults.qml")

  Component.onCompleted: {
    searchBar.query = searchResults.pageArgs.searchQuery;
    if (searchBar.query !== "") {
      isLoading = true;
      YT.requestSearch(searchBar.text);
    }
  }

  Connections {
    function onSearchSuccess(model) {
      resultsView.model = model;
      searchBar.focus = false
      searchResults.pageArgs.model = model;
      searchResults.isLoading = false;
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
      leftPadding: height * 0.9

      onAccepted: {
        searchResults.isLoading = true;
        YT.requestSearch(searchBar.text);
        searchResults.pageArgs.searchQuery = searchBar.text;
      }

      IconButton {
        id: backButton

        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        hasState: false
        height: Design.iconSize.small
        size: Qt.size(height, width)
        source: "qrc:/icons/left-arrow.svg"
        width: Design.iconSize.small

        background: Rectangle {
          color: palette.alternateBase
          radius: parent.height / 2
        }

        onClicked: {
          rootContent.stack.pop(StackView.Immediate)
          rootContent.stack.currentItem.pageArgs.query = searchBar.text;
        }
      }
    }
    Item {
      id: skeletonContainer

      property real shimmerProgress: 0.0

      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.topMargin: 12

      NumberAnimation {
        duration: 1400
        from: 0.0
        loops: Animation.Infinite
        property: "shimmerProgress"
        running: searchResults.isLoading
        target: skeletonContainer
        to: 1.0
      }
      SongListView {
        id: resultsView

        anchors.fill: parent
        clip: true
        model: searchResults.pageArgs.model
        visible: !searchResults.isLoading
      }
      ListView {
        id: skeletonView

        anchors.fill: parent
        clip: true
        interactive: false
        model: Math.floor(3440 / 80)
        spacing: resultsView.spacing
        visible: searchResults.isLoading

        delegate: SongItemSkeleton {
          globalProgress: skeletonContainer.shimmerProgress
          height: 64
          width: parent.width
        }
      }
    }
  }
}
