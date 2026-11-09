import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Vermilion

VmModal {
  id: root

  property string coverImage
  property string negativeButtonText: "Cancel"
  property string positiveButtonText: "Create"
  property string title

  signal playlistCreated(string plId)

  onClosed: {
    root.title = ""
    root.coverImage = ""
  }

  function createPlaylist() {
    if (title.trim().length > 0) {
      const id = PlaylistsController.createPlaylist(root.title);
      if (root.coverImage !== "")
        PlaylistsController.updatePlaylistArt(id, root.coverImage);
      root.close();
      root.playlistCreated(id);
    }
  }

  height: 211
  width: 358

  Column {
    anchors.fill: parent
    anchors.margins: 16
    spacing: 16

    Text {
      id: headLine
      color: palette.text
      font.pixelSize: Design.fontSize.base
      font.weight: Font.Bold
      text: "New Playlist"
    }
    Column {
      id: textInputCol

      spacing: 8

      width: parent.width
      Column {
        id: titleInputCol

        spacing: 4

        width: parent.width
        TextField {
          font.pixelSize: Design.fontSize.small
          height: Design.fontSize.base * 1.5
          placeholderText: "Title"
          placeholderTextColor: Qt.lighter(palette.alternateBase, 4)
          text: root.title
          width: parent.width

          background: Rectangle {
            color: "transparent"
            implicitHeight: 100
            implicitWidth: 100
          }

          onTextChanged: {
            root.title = text;
          }
        }
        UnderLine {
        }
      }
      Column {
        id: coverInputCol

        spacing: 4
        width: parent.width

        Item {
          height: 40
          width: parent.width

          RoundedImage {
            id: coverImage

            height: parent.height
            source: root.coverImage !== "" ? root.coverImage : "qrc:/images/default-playlist-cover.png"
            width: parent.height
          }
          Text {
            id: coverImagePath

            anchors.left: coverImage.right
            anchors.leftMargin: 8
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            color: root.coverImage !== "" ? palette.text : Qt.lighter(palette.alternateBase, 4)
            elide: Text.ElideLeft
            font.pixelSize: Design.fontSize.small
            height: Design.fontSize.base * 1.5
            text: root.coverImage !== "" ? root.coverImage : "Cover (optional)"
          }
          MouseArea {
            anchors.fill: parent

            onClicked: {
              coverImagePath.focus = false;
              coverPickerDialog.open();
            }
          }
        }
        UnderLine {
        }
      }
    }
    Item {
      id: buttonsContainer
      height: 40
      width: parent.width

      Button {
        id: negativeButton

        anchors.left: parent.left
        font.pixelSize: Design.fontSize.small
        height: parent.height
        text: root.negativeButtonText
        width: implicitWidth * 2

        background: ButtonBackground {
          color: palette.window
        }

        onClicked: {
          root.close();
        }
      }
      Button {
        id: positiveButton

        anchors.left: negativeButton.right
        anchors.leftMargin: 8
        anchors.right: parent.right
        height: parent.height
        width: implicitWidth * 1.5

        background: ButtonBackground {
          color: palette.accent
        }
        contentItem: Text {
          anchors.fill: parent
          color: palette.window
          font.pixelSize: Design.fontSize.small
          horizontalAlignment: Text.AlignHCenter
          text: root.positiveButtonText
          verticalAlignment: Text.AlignVCenter
        }

        onClicked: {
          root.createPlaylist();
        }
      }
    }
  }
  FileDialog {
    id: coverPickerDialog

    currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]

    onAccepted: {
      root.coverImage = selectedFile;
    }
  }

  component UnderLine: Rectangle {
    color: Qt.lighter(palette.alternateBase, 2)
    height: 1
    width: parent.width
  }

  component ButtonBackground: Rectangle {
    radius: 8
  }
}
