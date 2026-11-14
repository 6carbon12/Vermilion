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
    root.open();
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
  background: Item {
  }
  contentItem: Rectangle {
    id: content

    property size iconSize: Qt.size(Design.iconSize.base, Design.iconSize.base)
    property int largeSpacing: 30
    property int mediumSpacing: 24

    color: palette.window
    height: contentLayout.implicitHeight

    ColumnLayout {
      id: contentLayout

      anchors.left: parent.left
      anchors.right: parent.right
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

          Layout.preferredHeight: Design.iconSize.base * 2
          Layout.preferredWidth: Design.iconSize.base * 2
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

        spacing: content.mediumSpacing

        RowLayout {
          id: bigSongControls

          Layout.leftMargin: content.mediumSpacing
          Layout.preferredHeight: 96
          Layout.rightMargin: content.mediumSpacing
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

          Layout.fillWidth: true

          SmallMenuItem {
            id: addToPlaylist

            icon: "qrc:/icons/add-to-playlist.svg"
            text: "Add to playlist"

            TapHandler {
              onSingleTapped: {
                plAddModal.close();
                plAddModal.openForTrack(root.track);
              }
            }
          }
          SmallMenuItem {
            id: download

            icon: "qrc:/icons/download.svg"
            text: "Download"

            TapHandler {
              onSingleTapped: {
                console.log(parent.text);
              }
            }
          }
          SmallMenuItem {
            id: openArtist

            icon: "qrc:/icons/info.svg"
            text: "Open artist"

            TapHandler {
              onSingleTapped: {
                console.log(parent.text);
              }
            }
          }
          SmallMenuItem {
            id: removeFromPlaylist

            clickedColor: Qt.alpha(iconTextColor, 0.2)
            hoverColor: Qt.alpha(iconTextColor, 0.1)
            icon: "qrc:/icons/trash.svg"
            iconTextColor: "#FF8183"
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
  enter: Transition {
    NumberAnimation {
      duration: 150
      easing.type: Easing.OutCubic
      from: root.height
      property: "y"
      target: content
      to: 0
    }
  }
  exit: Transition {
    NumberAnimation {
      duration: 150
      easing.type: Easing.InCubic
      property: "y"
      target: content
      to: root.height
    }
  }

  PlaylistAdditionModal {
    id: plAddModal
  }

  component SmallMenuItem: Rectangle {
    id: menuItem

    property color clickedColor: Qt.lighter(palette.alternateBase, 1.25)
    property color hoverColor: Qt.lighter(palette.alternateBase, 1.5)
    property string icon
    property color iconTextColor: palette.text
    property string text

    anchors.left: parent.left
    anchors.right: parent.right
    color: menuItemTapHandler.pressed ? menuItem.clickedColor : menuItemHoverHandler.hovered ? menuItem.hoverColor : "transparent"
    height: _menuItem.implicitHeight + (content.mediumSpacing)
    implicitHeight: _menuItem.implicitHeight
    implicitWidth: _menuItem.implicitHeight
    width: _menuItem.implicitWidth

    Behavior on color {
      ColorAnimation {
        duration: 200
      }
    }

    ContextMenuItem {
      id: _menuItem

      anchors.left: parent.left
      anchors.leftMargin: content.mediumSpacing
      anchors.verticalCenter: parent.verticalCenter
      color: menuItem.iconTextColor
      icon: menuItem.icon
      iconSize: content.iconSize
      spacing: content.mediumSpacing
      text: menuItem.text
    }
    HoverHandler {
      id: menuItemHoverHandler
    }
    TapHandler {
      id: menuItemTapHandler
    }
  }
}
