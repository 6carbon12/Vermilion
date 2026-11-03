pragma ComponentBehavior: Bound
import QtCore

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Effects
import QtQuick.Layouts
import Vermilion
import "../components"

Popup {
  id: root

  property int numTracks
  property Playlist playlist

  signal editNameRequested

  function deletePlaylist() {
    PlaylistsController.deletePlaylist(root.playlist.id);
    deleteConfirmationModal.close();
    root.close();
    rootContent.stack.pop();
  }
  function editName() {
    root.close();
    editNameRequested();
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
    property int smallSpacing: 12

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
        id: playlistInfoLayout

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
            text: root.playlist.name
            width: parent.width
            wrapMode: Text.WordWrap
          }
          Text {
            color: Qt.darker(palette.text, 2)
            font.pixelSize: Design.fontSize.base
            text: root.numTracks + " songs • 2hr 34min"
          }
        }
        Item {
          id: playlistInfoMiddleSpacer

          Layout.fillWidth: true
        }
        IconButton {
          Layout.preferredHeight: 28
          Layout.preferredWidth: 28
          hasState: false
          source: Qt.resolvedUrl("../../../assets/icons/cross.svg")

          onClicked: {
            root.close();
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
        id: playlistControlsLayout

        Layout.leftMargin: content.mediumSpacing
        Layout.rightMargin: content.mediumSpacing
        spacing: content.mediumSpacing

        RowLayout {
          id: bigPlaylistControls

          Layout.preferredHeight: 128
          spacing: content.mediumSpacing

          BigButton {
            Layout.fillHeight: true
            Layout.fillWidth: true
            iconSource: Qt.resolvedUrl("../../../assets/icons/play-next.svg")
            size: content.iconSize

            onClicked: {
              console.log("[ACTION] Add playlist as the next item.");
            }
          }
          BigButton {
            Layout.fillHeight: true
            Layout.fillWidth: true
            iconSource: Qt.resolvedUrl("../../../assets/icons/play-last.svg")
            size: content.iconSize

            onClicked: {
              console.log("[ACTION] Add playlist as the last item.");
            }
          }
        }
        Column {
          id: smallPlaylistControls

          Layout.bottomMargin: content.mediumSpacing
          spacing: content.largeSpacing

          ContextMenuItem {
            icon: Qt.resolvedUrl("../../../assets/icons/shuffle.svg")
            iconSize: content.iconSize
            spacing: content.mediumSpacing
            text: "Shuffle play"

            TapHandler {
              onSingleTapped: {
                console.log(parent.text);
              }
            }
          }
          ContextMenuItem {
            icon: Qt.resolvedUrl("../../../assets/icons/note-pencil.svg")
            iconSize: content.iconSize
            spacing: content.mediumSpacing
            text: "Edit name"

            TapHandler {
              onSingleTapped: {
                root.editName();
              }
            }
          }
          ContextMenuItem {
            icon: Qt.resolvedUrl("../../../assets/icons/image.svg")
            iconSize: content.iconSize
            spacing: content.mediumSpacing
            text: "Change cover"

            TapHandler {
              onSingleTapped: {
                artFilePickDialog.open();
              }
            }
          }
          ContextMenuItem {
            icon: Qt.resolvedUrl("../../../assets/icons/info.svg")
            iconSize: content.iconSize
            spacing: content.mediumSpacing
            text: "Info"

            TapHandler {
              onSingleTapped: {
                console.log(parent.text);
              }
            }
          }
          ContextMenuItem {
            color: "#FF8183"
            icon: Qt.resolvedUrl("../../../assets/icons/trash.svg")
            iconSize: content.iconSize
            spacing: content.mediumSpacing
            text: "Remove from library"

            TapHandler {
              onSingleTapped: {
                deleteConfirmationModal.open();
              }
            }
          }
        }
      }
    }
  }

  VmModal {
    id: deleteConfirmationModal

    height: width * (2 / 3)
    width: 256 * 1.5

    contentItem: ColumnLayout {
      id: mainCol

      anchors.fill: parent
      anchors.margins: 16
      spacing: anchors.margins / 2

      ColumnLayout {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        clip: true

        Image {
          Layout.alignment: Qt.AlignHCenter
          Layout.preferredHeight: Design.iconSize.large / 1.6
          Layout.preferredWidth: Design.iconSize.large / 1.6
          layer.enabled: true
          source: Qt.resolvedUrl("../../../assets/icons/info.svg")
          sourceSize.height: Layout.preferredHeight
          sourceSize.width: Layout.preferredWidth

          layer.effect: MultiEffect {
            brightness: 1.0
            colorization: 1.0
            colorizationColor: "#FF8183"
          }
        }
        Column {
          Layout.preferredWidth: mainCol.width * 0.8

          Text {
            color: palette.text
            font.pixelSize: Design.fontSize.large
            horizontalAlignment: Text.AlignHCenter
            text: "Delete Playlist"
            width: parent.width
          }
          Text {
            color: Qt.darker(palette.text, 2)
            font.pixelSize: Design.fontSize.base
            font.weight: Font.Medium
            horizontalAlignment: Text.AlignHCenter
            text: "Are your sure you want to delete the playlist \"" + root.playlist.name + "\"?"
            width: parent.width
            wrapMode: Text.WordWrap
          }
        }
      }
      RowLayout {
        Layout.fillWidth: true
        clip: true
        spacing: mainCol.anchors.margins * 0.75

        Button {
          Layout.fillWidth: true
          Layout.preferredHeight: 48

          background: Rectangle {
            color: "black"
            radius: 8
          }
          contentItem: Text {
            color: palette.text
            font.pixelSize: Design.fontSize.base
            font.weight: Font.Medium
            horizontalAlignment: Qt.AlignHCenter
            text: "Cancel"
            verticalAlignment: Qt.AlignVCenter
          }

          onClicked: {
            deleteConfirmationModal.close();
          }
        }
        Button {
          Layout.fillWidth: true
          Layout.preferredHeight: 48

          background: Rectangle {
            color: "#FF8183"
            radius: 8
          }
          contentItem: Text {
            color: "black"
            font.pixelSize: Design.fontSize.base
            horizontalAlignment: Qt.AlignHCenter
            text: "Delete"
            verticalAlignment: Qt.AlignVCenter
          }

          onClicked: {
            root.deletePlaylist();
          }
        }
      }
    }
  }
  FileDialog {
    id: artFilePickDialog

    currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]

    onAccepted: {
      PlaylistsController.updatePlaylistArt(root.playlist.id, selectedFile);
    }
  }
}
