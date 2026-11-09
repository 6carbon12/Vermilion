pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vermilion
import "../components"

Popup {
  id: root

  property bool fromPlaylist: false
  property Playlist pl
  property Track track

  function openForTrack(newTrack) {
    track = newTrack;
    open();
  }

  height: contentLayout.implicitHeight
  modal: true
  padding: 0
  parent: Overlay.overlay
  width: parent.width
  y: parent.height - height
  z: 10

  Overlay.modal: Rectangle {
    color: Qt.alpha(palette.window, 0.5)
  }
  contentItem: Rectangle {
    id: content

    property size iconSize: Qt.size(Design.iconSize.base, Design.iconSize.base)
    property int largeSpacing: 30
    property int mediumSpacing: 24

    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    color: palette.window
    height: contentLayout.implicitHeight

    ColumnLayout {
      id: contentLayout

      anchors.fill: parent
      spacing: content.mediumSpacing

      RowLayout {
        id: songInfoLayout

        Layout.fillWidth: true
        Layout.leftMargin: content.mediumSpacing
        Layout.rightMargin: content.mediumSpacing
        Layout.topMargin: content.mediumSpacing

        Column {
          Layout.fillWidth: true
          clip: true
          spacing: 0

          Text {
            color: palette.text
            font.pixelSize: Design.fontSize.large
            font.weight: 500
            text: root.track.title
            width: parent.width
            wrapMode: Text.WordWrap
          }
          Row {
            Text {
              color: Qt.darker(palette.text, 2)
              font.pixelSize: Design.fontSize.small * 1.2
              text: root.track.views + " • "
            }
            ArtistsText {
              artists: root.track.artists
              fontSize: Design.fontSize.small * 1.2
              textColor: Qt.darker(palette.text, 2)
            }
          }
        }
        Item {
          id: songInfoMiddleSpacer

          Layout.fillWidth: true
        }
        IconButton {
          id: likeButton

          Layout.preferredHeight: 28
          Layout.preferredWidth: 28
          // TODO: Add proper way to get to know if song is liked
          isActive: root.track && PlaylistsController.isLiked(root.track.id)
          source: "qrc:/icons/heart"

          onClicked: {
            PlaylistsController.toggleLike(root.track);
          }

          Connections {
            function onTracksUpdated(plId) {
              if (plId == "liked") {
                likeButton.isActive = PlaylistsController.isLiked(root.track.id);
              }
            }

            target: PlaylistsController
          }
          Connections {
            function onTrackChanged() {
              likeButton.isActive = PlaylistsController.isLiked(root.track.id);
            }

            target: root
          }
        }
      }
      Rectangle {
        id: separator

        Layout.fillWidth: true
        Layout.maximumHeight: 2
        Layout.preferredHeight: 2
        color: palette.alternateBase
      }
      ColumnLayout {
        id: songControlsLayout

        Layout.leftMargin: content.mediumSpacing
        Layout.rightMargin: content.mediumSpacing
        spacing: content.mediumSpacing

        RowLayout {
          id: bigSongControls

          Layout.preferredHeight: 128
          spacing: content.mediumSpacing

          BigButton {
            Layout.fillHeight: true
            Layout.fillWidth: true
            iconSource: "qrc:/icons/play-next.svg"
            size: content.iconSize

            onClicked: {
              console.log("[ACTION] PLAY NEXT.");
            }
          }
          BigButton {
            Layout.fillHeight: true
            Layout.fillWidth: true
            iconSource: "qrc:/icons/play-last.svg"
            size: content.iconSize

            onClicked: {
              console.log("[ACTION] ADD TO QUEUE.");
            }
          }
        }
        Column {
          id: smallSongControls

          Layout.bottomMargin: content.mediumSpacing
          spacing: content.largeSpacing

          ContextMenuItem {
            icon: "qrc:/icons/add-to-playlist.svg"
            iconSize: content.iconSize
            spacing: content.mediumSpacing
            text: "Add to library"

            TapHandler {
              onSingleTapped: {
                plAddModal.close();
                plAddModal.openForTrack(root.track)
              }
            }
          }
          ContextMenuItem {
            icon: "qrc:/icons/download.svg"
            iconSize: content.iconSize
            spacing: content.mediumSpacing
            text: "Download"

            TapHandler {
              onSingleTapped: {
                console.log(parent.text);
              }
            }
          }
          ContextMenuItem {
            icon: "qrc:/icons/info.svg"
            iconSize: content.iconSize
            spacing: content.mediumSpacing
            text: "Open Artist"

            TapHandler {
              onSingleTapped: {
                console.log(parent.text);
              }
            }
          }
          ContextMenuItem {
            color: "#FF8183"
            icon: "qrc:/icons/trash.svg"
            iconSize: content.iconSize
            spacing: content.mediumSpacing
            text: "Remove"
            visible: root.fromPlaylist

            TapHandler {
              onSingleTapped: {
                PlaylistsController.removeFromPlaylist(root.track.id, root.pl.id);
                root.close();
              }
            }
          }
        }
      }
    }
  }

  PlaylistAdditionModal {
    id: plAddModal
  }
}
