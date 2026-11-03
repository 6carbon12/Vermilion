pragma ComponentBehavior: Bound

import QtQuick
import Vermilion
import "../components"

ListView {
  id: tracksList

  boundsMovement: Flickable.StopAtBounds
  spacing: 16

  delegate: SongItem {
    id: songItem

    required property list<Artist> artists
    required property string url

    height: 64
    width: parent.width

    rightContent: IconButton {
      hasState: false
      source: Qt.resolvedUrl("../../../assets/icons/three-dot-vertical.svg")

      onClicked: {
        console.log("[ACTION] Open song context menu");
      }
    }
    subText: Row {
      ArtistsText {
        id: artists

        artists: songItem.artists
        fontSize: 14
        textColor: Qt.darker(palette.text, 2)
      }
      Text {
        id: separator

        color: Qt.darker(palette.text, 2)
        font.pixelSize: 14
        text: " • "
      }
      Text {
        id: duration

        color: Qt.darker(palette.text, 2)
        font.pixelSize: 14
        text: songItem.duration
      }
    }

    MouseArea {
      anchors.fill: parent

      onClicked: {
        console.log("[ACTION] Play song");
        Player.play(songItem.url);
      }
    }
  }
}
