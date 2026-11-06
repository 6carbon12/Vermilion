pragma ComponentBehavior: Bound

import QtQuick
import Vermilion
import "../components"

ListView {
  id: tracksList

  property bool fromPlaylist: false
  property Playlist pl

  boundsMovement: Flickable.StopAtBounds
  spacing: 16

  delegate: SongItem {
    id: songItem

    required property list<Artist> artists
    required property Track track
    required property string url

    height: 64
    width: tracksList.width

    rightContent: IconButton {
      hasState: false
      source: "qrc:/icons/three-dot-vertical.svg"

      // Need this since `rightContentClicked` is not emitted when a control is passed.
      onClicked: {
        contextMenu.close();
        contextMenu.track = songItem.track;
        contextMenu.open();
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

    onMainContentClicked: {
      Player.play(url);
    }
  }

  SongItemContextMenu {
    id: contextMenu

    fromPlaylist: tracksList.fromPlaylist
    pl: tracksList.pl
  }
}
