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

  padding: 0
  pageArgs: ({})
  pageUrl: Qt.resolvedUrl("PlaylistView.qml")

  background: Rectangle {
    color: "transparent"

    Rectangle {
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.verticalCenter: parent.top
      color: "transparent"
      height: 860

      transform: Translate {
        y: -flickable.contentY
      }

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
    function onTracksUpdated(playlistId) {
      if (root.pt.id === playlistId) {
        root.model = PlaylistsController.getTracksFromPlaylist(root.pt.id);
      }
    }

    target: PlaylistsController
  }
  Connections {
    function onPlaylistUpdated(playlistId) {
      if (root.pt.id == playlistId) {
        console.log("Playlist has been updated.");
        root.pageArgs.playlist = PlaylistsController.getPlaylistFromId(playlistId);
        root.pt = root.pageArgs.playlist;
        root.model = PlaylistsController.getTracksFromPlaylist(playlistId);
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

  // Floating Header directly on BasePage
  Rectangle {
    id: topBarBg

    readonly property real fadeEnd: Math.max(1, tracksList.y - height)
    readonly property real fadeStart: 0
    readonly property real scrollProgress: Math.min(1.0, Math.max(0.0, (flickable.contentY - fadeStart) / (fadeEnd - fadeStart)))

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    color: Qt.rgba(palette.window.r, palette.window.g, palette.window.b, topBarBg.scrollProgress)
    height: 64
    z: 10

    TwoButtonBar {
      id: topBar

      // Header offsets offset by topPadding (64)
      readonly property real nameLayoutYInMain: playlistArt.Layout.preferredHeight + playlistInfo.spacing
      readonly property real textFadeEnd: Math.max(textFadeStart + 1, nameLayoutYInMain + playlistNameLayout.implicitHeight)
      readonly property real textFadeStart: Math.max(0, nameLayoutYInMain)

      anchors.fill: parent
      anchors.margins: 8
      anchors.verticalCenter: parent.verticalCenter
      leftIcon: "qrc:/icons/left-arrow.svg"
      middleText: root.pt.name
      middleTextColor: palette.text
      middleTextOpacity: Math.min(1.0, Math.max(0.0, (flickable.contentY - textFadeStart) / (textFadeEnd - textFadeStart)))
      rightIcon: "qrc:/icons/three-dot-vertical.svg"

      onLeftClicked: rootContent.stack.pop()
      onRightClicked: topRightContextMenu.open()
    }
  }

  // Scrollable Content filling parent underneath topBarBg
  Flickable {
    id: flickable

    anchors.fill: parent
    boundsBehavior: Flickable.DragOverBounds
    clip: true
    contentHeight: mainLayout.implicitHeight + topPadding
    contentWidth: width
    property int topPadding: topBarBg.height

    ColumnLayout {
      id: mainLayout
      anchors.topMargin: flickable.topPadding
      anchors.fill: parent
      anchors.leftMargin: 16
      anchors.rightMargin: 16

      spacing: 20
      width: parent.width

      ColumnLayout {
        id: playlistInfo

        Layout.fillWidth: true
        spacing: 16

        RoundedImage {
          id: playlistArt

          Layout.alignment: Qt.AlignHCenter
          Layout.preferredHeight: 256
          Layout.preferredWidth: 256
          source: root.pageArgs.playlist.artUrl
        }
        ColumnLayout {
          id: playlistNameLayout

          Layout.alignment: Qt.AlignHCenter
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
        Layout.preferredHeight: 64
        spacing: 12

        Item {
          Layout.fillWidth: true
        }
        CircleIconButton {
          id: downloadButton

          Layout.alignment: Qt.AlignHCenter
          iconSource: "qrc:/icons/download.svg"
          margins: 12

          onClicked: console.log("[ACTION] Download Playlist:", root.pt.id)
        }
        IconButton {
          id: playButton

          hasState: false
          size: Qt.size(Design.iconSize.large, Design.iconSize.large)
          source: "qrc:/icons/play-circle.svg"

          onClicked: Player.playPlaylist(root.pt)
        }
        CircleIconButton {
          id: editButton

          Layout.alignment: Qt.AlignHCenter
          iconSource: "qrc:/icons/pen.svg"
          margins: 12

          onClicked: console.log("[ACTION] Edit Playlist:", root.pt.id)
        }
        Item {
          Layout.fillWidth: true
        }
      }
      PlaylistSortByDropdown {
        Layout.fillWidth: true
        Layout.preferredHeight: 48
        pt: root.pt
        spacing: 16
      }
      SongListView {
        id: tracksList

        Layout.fillWidth: true
        fromPlaylist: true
        implicitHeight: contentHeight
        interactive: false
        model: root.model
        pl: root.pt
        Layout.bottomMargin: 80 + spacing
      }
    }
  }
}
