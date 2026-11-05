pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vermilion
import "../components"

VmModal {
  id: root
  property Track track;

  property var selectedPlaylistIds: []

  clip: true
  height: 662
  width: height * (3 / 5)

  contentItem: Rectangle {
    anchors.fill: parent
    clip: true
    color: "transparent"

    ColumnLayout {
      id: mainCol

      anchors.left: parent.left
      anchors.margins: 24
      anchors.right: parent.right
      anchors.top: parent.top
      spacing: anchors.margins / 2

      Rectangle {
        id: modalTopBar

        Layout.fillWidth: true
        Layout.preferredHeight: 32
        color: 'transparent'

        Text {
          anchors.centerIn: parent
          color: palette.text
          font.pixelSize: Design.fontSize.base
          font.weight: Font.Medium
          text: "Add to a playlist"
        }
      }
      Column {
        id: modalContentCol

        Layout.fillHeight: true
        Layout.fillWidth: true
        spacing: 16

        VmTextField {
          id: searchBar

          height: 48
          placeholderText: "Search Playlist..."
          width: mainCol.width

          IconButton {
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            hasState: false
            height: Design.iconSize.small
            size: Qt.size(height, width)
            source: Qt.resolvedUrl("../../../assets/icons/search-inactive.svg")
            width: Design.iconSize.small
          }
        }
        Column {
          id: playlists

          property alias selectedPlaylistIds: root.selectedPlaylistIds

          function togglePlaylist(id) {
            let temp = [...selectedPlaylistIds];
            let idx = temp.indexOf(id);
            if (idx !== -1) {
              temp.splice(idx, 1);
            } else {
              temp.push(id);
            }
            selectedPlaylistIds = temp;
          }

          spacing: 16

          onSelectedPlaylistIdsChanged: {
            console.log(selectedPlaylistIds);
          }

          Column {
            id: recents

            Text {
              id: recentsText

              color: palette.text
              font.pixelSize: Design.fontSize.base
              font.weight: Font.Bold
              text: "Recents"
            }
            PlaylistChecklist {
              id: recentPlaylistsView

              height: 64 * 3
              model: PlaylistsController.model
              num: 3
              selectedIds: playlists.selectedPlaylistIds
              width: mainCol.width

              onItemToggled: id => playlists.togglePlaylist(id)
            }
          }
          Column {
            id: allPlaylists

            spacing: 4

            Text {
              id: allPlaylistsText

              color: palette.text
              font.pixelSize: Design.fontSize.base
              font.weight: Font.Bold
              text: "All Playlists"
            }
            PlaylistChecklist {
              id: allPlaylistsView

              height: 64 * 3
              model: PlaylistsController.model
              selectedIds: playlists.selectedPlaylistIds
              width: mainCol.width

              header: CheckboxRow {
                height: 64
                text: "Add new Playlist"
                width: mainCol.width

                checkBoxBackground: IconButton {
                  hasState: false
                  size: Qt.size(Design.iconSize.small, Design.iconSize.small)
                  source: Qt.resolvedUrl("../../../assets/icons/plus.svg")

                  background: Rectangle {
                    color: palette.window
                    radius: 8
                  }
                }
              }

              onItemToggled: id => playlists.togglePlaylist(id)
            }
          }
        }
      }
    }
    Rectangle {
      anchors.bottom: parent.bottom
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.top: mainCol.bottom
      anchors.topMargin: 16
      color: Qt.darker(palette.alternateBase, 1.2)
      radius: 8

      Rectangle {
        color: parent.color
        height: parent.radius

        anchors {
          left: parent.left
          right: parent.right
          top: parent.top
        }
      }
      Column {
        anchors.fill: parent
        height: Design.fontSize.base + 24

        Rectangle {
          id: topBorder

          anchors.left: parent.left
          anchors.right: parent.right
          color: Qt.darker(palette.text, 3)
          height: 1
        }
        Rectangle {
          id: spacer

          anchors.left: parent.left
          anchors.right: parent.right
          color: "transparent"
          height: 12
        }
        RowLayout {
          id: controls

          anchors.left: parent.left
          anchors.leftMargin: 24
          anchors.right: parent.right
          anchors.rightMargin: 24
          height: Design.fontSize.base + 20
          spacing: 12

          Button {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: 1

            background: Rectangle {
              color: palette.window
              radius: 8
            }
            contentItem: Text {
              anchors.fill: parent
              color: palette.text
              font.pixelSize: Design.fontSize.small
              horizontalAlignment: Text.AlignHCenter
              text: "Cancel"
              verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
              root.close();
            }
          }
          Button {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: 1

            background: Rectangle {
              color: palette.accent
              radius: 8
            }
            contentItem: Text {
              anchors.fill: parent
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
}
