pragma ComponentBehavior: Bound

import QtQuick
import Vermilion

ListView {
  id: root
  boundsMovement: Flickable.StopAtBounds
  clip: true
  property int num
  property Track track;
  delegate: CheckboxRow {
    id: plItem

    required property int index
    required property Playlist playlist

    height: root.num ? index < root.num ? 64 : 0 : 64
    selected: PlaylistsController.isTrackInPlaylist(root.track.id, plItem.playlist.id);
    text: plItem.playlist.name
    width: ListView.view.width

    checkBoxBackground: RoundedImage {
      source: plItem.playlist.artUrl
    }

    Connections {
      target: root

      function onTrackChanged(plId) {
        plItem.selected = PlaylistsController.isTrackInPlaylist(root.track.id, plItem.playlist.id);
      }
    }

    Connections {
      target: PlaylistsController

      function onTracksUpdated(plId) {
        plItem.selected = PlaylistsController.isTrackInPlaylist(root.track.id, plItem.playlist.id);
      }
    }

    onClicked: {
      if (plItem.selected) {
        PlaylistsController.removeFromPlaylist(root.track.id, plItem.playlist.id);
      } else {
        PlaylistsController.addToPlaylist(root.track, plItem.playlist.id);
      }
    }
  }
}
