pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vermilion

VmModal {
  id: root

  property var selectedPlaylistIds: []
  property Track track

  function openForTrack(newTrack) {
    track = newTrack;
    trackChanged();
    open();
  }

  clip: true
  width: 397 * 0.8

  contentItem: Item {
    // This tells the modal exactly how tall it needs to be
    implicitHeight: layoutCol.implicitHeight

    Column {
      id: layoutCol

      width: parent.width

      // Top Section (Main Content)
      Column {
        id: mainContelCol
        bottomPadding: 16
        spacing: 12
        topPadding: 12
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 24
        anchors.rightMargin: 24

        Item {
          height: 32
          width: parent.width

          Text {
            anchors.centerIn: parent
            color: palette.text
            font.pixelSize: Design.fontSize.base
            font.weight: Font.Medium
            text: "Add to a playlist"
          }
        }
        VmTextField {
          id: searchBar

          height: 48
          placeholderText: "Search Playlist..."
          width: parent.width

          IconButton {
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            hasState: false
            height: Design.iconSize.small
            size: Qt.size(height, width)
            source: "qrc:/icons/search-inactive.svg"
            width: height
          }
        }
        Column {
          id: playlists

          spacing: 16
          width: parent.width

          Column {
            id: recents

            visible: recentPlaylistsView.count > 0
            width: parent.width

            Text {
              color: palette.text
              font.pixelSize: Design.fontSize.base
              font.weight: Font.Bold
              text: "Recents"
            }
            PlaylistChecklist {
              id: recentPlaylistsView

              height: 64 * Math.min(count, 3)
              model: PlaylistsController.userPlaylists
              num: 3
              track: root.track
              width: parent.width
            }
          }
          Column {
            id: allPlaylists

            spacing: 4
            width: parent.width

            Text {
              color: palette.text
              font.pixelSize: Design.fontSize.base
              font.weight: Font.Bold
              text: "All Playlists"
            }
            PlaylistChecklist {
              id: allPlaylistsView

              height: 64 * (count < 3 ? count + 1 : 3)
              model: PlaylistsController.userPlaylists
              track: root.track
              width: parent.width

              header: CheckboxRow {
                height: 64
                text: "Add new Playlist"
                width: parent.width

                checkBoxBackground: IconButton {
                  hasState: false
                  size: Qt.size(Design.iconSize.small, Design.iconSize.small)
                  source: "qrc:/icons/plus.svg"

                  background: Rectangle {
                    color: palette.window
                    radius: 8
                  }
                }

                onClicked: playlistCreationModal.open()
              }
            }
          }
        }
      }

      // Bottom Action Bar
      Rectangle {
        color: Qt.darker(palette.alternateBase, 1.2)
        // Manually calculate height based on the contents to avoid stretching
        height: 13 + (Design.fontSize.base + 20) + 16
        radius: 8
        width: parent.width

        // Square off the top corners
        Rectangle {
          anchors.top: parent.top
          color: parent.color
          height: parent.radius
          width: parent.width
        }
        Rectangle {
          id: topBorder

          color: Qt.darker(palette.text, 3)
          height: 1
          width: parent.width
        }
        RowLayout {
          anchors.bottomMargin: 16
          anchors.fill: parent
          anchors.leftMargin: 24
          anchors.rightMargin: 24
          anchors.topMargin: 13
          spacing: 12

          Button {
            Layout.fillHeight: true
            Layout.fillWidth: true

            background: Rectangle {
              color: palette.window
              radius: 8
            }
            contentItem: Text {
              color: palette.text
              font.pixelSize: Design.fontSize.small
              horizontalAlignment: Text.AlignHCenter
              text: "Cancel"
              verticalAlignment: Text.AlignVCenter
            }

            onClicked: root.close()
          }
          Button {
            Layout.fillHeight: true
            Layout.fillWidth: true

            background: Rectangle {
              color: palette.accent
              radius: 8
            }
            contentItem: Text {
              color: palette.window
              font.pixelSize: Design.fontSize.small
              font.weight: Font.Medium
              horizontalAlignment: Text.AlignHCenter
              text: "Done"
              verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
              for (let i = 0; i < root.selectedPlaylistIds.length; i++) {
                PlaylistsController.addToPlaylist(root.track, root.selectedPlaylistIds[i]);
              }
              root.close();
            }
          }
        }
      }
    }
  }

  PlaylistCreationModal {
    id: playlistCreationModal

    onPlaylistCreated: plId => {
      PlaylistsController.addToPlaylist(root.track, plId);
    }
  }
}
