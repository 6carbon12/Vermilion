pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts
import Vermilion
import "../components"
import "../widgets"

BasePage {
  id: root

  property TrackListModel model: PlaylistsController.getTracksFromPlaylist(root.pt.id)
  property Playlist pt: pageArgs.playlist

  pageArgs: ({})
  pageUrl: Qt.resolvedUrl("Playlist.qml")

  background: Rectangle {
    color: "transparent"

    Rectangle {
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.verticalCenter: parent.top
      color: "transparent"
      height: 860

      Image {
        id: backgroundImage

        anchors.fill: parent
        cache: true
        clip: true
        fillMode: Image.PreserveAspectCrop
        source: root.pt.artUrl
        sourceSize.height: 200
        sourceSize.width: parent.width
        visible: false
      }
      MultiEffect {
        anchors.fill: backgroundImage
        autoPaddingEnabled: false
        blur: 1.0
        blurEnabled: true
        blurMax: 80
        source: backgroundImage
      }
      Rectangle {
        anchors.fill: parent

        gradient: Gradient {
          GradientStop {
            color: Qt.rgba(0, 0, 0, 0.5)
            position: 0.5
          }
          GradientStop {
            color: Qt.rgba(0, 0, 0, 1.0)
            position: 1.0
          }
        }
      }
    }
  }

  Connections {
    function onPlaylistUpdated(playlistId) {
      if (root.pt.id == playlistId) {
        console.log("Playlist has been updated.");
        root.pageArgs.playlist = PlaylistsController.getPlaylistFromId(playlistId);
        root.pt = root.pageArgs.playlist;
      }
    }

    target: PlaylistsController
  }
  TapHandler {
    onSingleTapped: {
      playlistName.focus = false;
    }
  }
  PlaylistContextMenu {
    id: topRightContextMenu

    numTracks: root.model.rowCount()
    playlist: root.pt

    onEditNameRequested: {
      playlistName.focus = true;
    }
  }
  ColumnLayout {
    id: mainLayout

    anchors.centerIn: parent
    anchors.fill: parent
    anchors.margins: 8
    spacing: 20

    TwoButtonBar {
      Layout.fillWidth: true
      Layout.preferredHeight: 32
      leftIcon: Qt.resolvedUrl("../../../assets/icons/left-arrow.svg")
      rightIcon: Qt.resolvedUrl("../../../assets/icons/three-dot-vertical.svg")

      onLeftClicked: {
        rootContent.stack.pop();
      }
      onRightClicked: {
        console.log("[ACTION] Open context menu.");
        topRightContextMenu.open();
      }
    }
    ColumnLayout {
      id: playlistInfo

      Layout.fillWidth: true
      spacing: 16

      // Need to make other elements be centered.
      Rectangle {
        id: infoFiller

        Layout.fillWidth: true
        Layout.preferredHeight: 0
      }
      RoundedImage {
        id: playlistArt

        Layout.alignment: Qt.AlignHCenter
        Layout.preferredHeight: 256
        Layout.preferredWidth: 256
        Layout.topMargin: -playlistInfo.spacing
        source: root.pageArgs.playlist.artUrl
      }
      ColumnLayout {
        id: playlistNameLayout

        Layout.alignment: Qt.AlignHCenter
        Layout.fillHeight: true
        spacing: 0

        TextField {
          id: playlistName

          Layout.alignment: Qt.AlignHCenter
          Layout.fillWidth: true
          color: palette.text
          font.bold: true
          font.pixelSize: 32
          horizontalAlignment: Text.AlignHCenter
          text: root.pt.name
          wrapMode: Text.WordWrap

          background: Rectangle {
            color: "transparent"
          }

          onAccepted: {
            if (text != root.pt.name) {
              PlaylistsController.renamePlaylist(root.pt.id, playlistName.text);
            }
            playlistName.focus = false;
          }
          onFocusChanged: {
            if (text != root.pt.name) {
              PlaylistsController.renamePlaylist(root.pt.id, playlistName.text);
            }
          }
        }
        Text {
          id: playlistSubtext

          Layout.alignment: Qt.AlignHCenter
          color: Qt.darker(palette.text, 2)
          font.pixelSize: 14
          text: root.model.rowCount() + " songs • 2hr 34min"
        }
      }
    }
    RowLayout {
      id: controlsLayout

      Layout.fillWidth: true
      Layout.maximumHeight: 128
      Layout.preferredHeight: 128
      Layout.topMargin: -8
      spacing: 12

      Item {
        id: controlsLeftSpacer

        Layout.fillWidth: true
      }
      CircleIconButton {
        id: downloadButton

        Layout.alignment: Qt.AlignHCenter
        Layout.fillHeight: true
        Layout.maximumHeight: Design.iconSize.base + (margins * 2)
        Layout.preferredHeight: Design.iconSize.base + (margins * 2)
        Layout.preferredWidth: height
        iconSource: Qt.resolvedUrl("../../../assets/icons/download.svg")
        margins: 12

        onClicked: {
          console.log("[ACTION] Download Playlist:", root.pt.id);
        }
      }
      IconButton {
        id: playButton

        Layout.fillHeight: true
        Layout.preferredWidth: height
        hasState: false
        size: Qt.size(Design.iconSize.large, Design.iconSize.large)
        source: Qt.resolvedUrl("../../../assets/icons/play-circle.svg")

        onClicked: {
          console.log("[ACTION] Play Playlist:", root.pt.id);
        }
      }
      CircleIconButton {
        id: editButton

        Layout.alignment: Qt.AlignHCenter
        Layout.fillHeight: true
        Layout.maximumHeight: Design.iconSize.base + (margins * 2)
        Layout.preferredHeight: Design.iconSize.base + (margins * 2)
        Layout.preferredWidth: height
        iconSource: Qt.resolvedUrl("../../../assets/icons/pen.svg")
        margins: 12

        onClicked: {
          console.log("[ACTION] Download Playlist:", root.pt.id);
        }
      }
      Item {
        id: controlsRightSpacer

        Layout.fillWidth: true
      }
    }
    ColumnLayout {
      id: trackAndSort

      Layout.fillHeight: true
      Layout.fillWidth: true
      clip: true
      spacing: 8

      RowLayout {
        id: sortBy

        spacing: 16

        Text {
          color: palette.text
          font.bold: true
          font.pixelSize: 16
          text: "Sort By:"
        }
        Text {
          color: palette.text
          font.pixelSize: 16
          text: "A-Z"
        }
      }
      SongListView {
        id: tracksList

        Layout.fillHeight: true
        Layout.fillWidth: true
        model: root.model
      }
    }
  }
}
