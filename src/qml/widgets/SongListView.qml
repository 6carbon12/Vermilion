pragma ComponentBehavior: Bound

import QtQuick
import Vermilion
import "../components"

ListView {
  id: tracksList

  property bool fromPlaylist: false
  property Playlist pl

  boundsBehavior: Flickable.StopAtBounds
  flickableDirection: Flickable.VerticalFlick

  delegate: SongItem {
    id: songItem

    required property list<Artist> artists
    required property Track track
    required property string url

    height: 64 + (songItem.topPadding * 2)
    width: tracksList.width
    hoverEnabled: !contextMenu.opened

    rightContent: IconButton {
      id: songItemRightButton
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
      spacing: 0
      width: parent.width

      ArtistsText {
        artists: songItem.artists
        fontSize: 14
        textColor: Qt.darker(palette.text, 2)
        width: Math.min(implicitWidth, parent.width - separator.width - duration.width)
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

    onLeftSwipeActionTriggered: {
      playlistAdditionModalOpener.start();
    }
    onMainContentClicked: {
      Player.play(url);
    }
    onRightContentClicked: {
      contextMenu.close();
      contextMenu.track = songItem.track;
      contextMenu.open();
    }
    onRightSwipeActionTriggered: {
      console.log("RIGHT");
    }

    Timer {
      id: playlistAdditionModalOpener

      interval: 100

      onTriggered: {
        playlistAdditionModal.close();
        playlistAdditionModal.track = songItem.track;
        playlistAdditionModal.open();
      }
    }
  }

  PlaylistAdditionModal {
    id: playlistAdditionModal
  }
  SongItemContextMenu {
    id: contextMenu

    fromPlaylist: tracksList.fromPlaylist
    pl: tracksList.pl
  }
}
