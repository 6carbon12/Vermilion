pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vermilion
import "../components"

BasePage {
  id: searchResults

  pageUrl: Qt.resolvedUrl("SearchResults.qml")

  Component.onCompleted: {
    searchBar.text = searchResults.pageArgs.searchQuery;
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
      model: searchResults.pageArgs.model

      delegate: Item {
        id: root

        required property string duration
        required property int index
        required property string thumbnailUrl
        required property string title
        required property string views

        height: 80
        width: ListView.view ? ListView.view.width : parent.width

        RowLayout {
          id: trackLayout

          anchors.fill: parent
          spacing: 16

          Rectangle {
            id: albumCoverContainer

            Layout.preferredHeight: 64
            Layout.preferredWidth: 64
            clip: true
            color: palette.alternateBase

            Image {
              id: albumCover

              anchors.fill: parent
              fillMode: Image.PreserveAspectCrop
              source: root.thumbnailUrl
            }
          }

          // Middle Text Column
          ColumnLayout {
            id: content

            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            spacing: 4

            Text {
              Layout.fillWidth: true
              color: "white"
              elide: Text.ElideRight
              font.pixelSize: 16
              font.weight: Font.Medium
              text: root.title
            }
            Text {
              Layout.fillWidth: true
              color: "#B3B3B3" // Light gray for subtitle text
              elide: Text.ElideRight
              font.pixelSize: 14
              text: root.duration + " • " + root.views
            }
          }

          // TODO: Add working items to this.
          Item {
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
              resultsView.model.playTrackAtIndex(root.index);
            }
          }
        }
      }
    }
  }
}
