import QtQuick
import QtQuick.Layouts
import Vermilion as Vermilion

// TODO: Add context menu when long pressed or right clicked.
Rectangle {
  id: root

  property alias font: playlistName.font
  required property Vermilion.Playlist playlist

  clip: true
  color: "transparent"

  ColumnLayout {
    anchors.fill: parent
    spacing: 8

    Item {
      id: imageContainer

      Layout.fillWidth: true
      Layout.preferredHeight: width

      RoundedImage {
        id: playlistCover

        anchors.fill: parent
        source: Qt.resolvedUrl(root.playlist.artUrl)
      }
    }
    Text {
      id: playlistName

      Layout.alignment: Qt.AlignTop
      Layout.fillWidth: true
      color: palette.text
      elide: Text.ElideRight
      text: root.playlist.name
    }
    Item {
      id: bottomSpacer

      Layout.fillHeight: true
    }
    TapHandler {
      onSingleTapped: {
        console.log("Opening playlist.")
        rootContent.stack.pushItem(Qt.resolvedUrl("../pages/PlaylistView.qml"), {
          pageArgs: {
            playlist: root.playlist
          }
        });
      }
    }
  }
}
