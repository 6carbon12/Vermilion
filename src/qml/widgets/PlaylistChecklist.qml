pragma ComponentBehavior: Bound

import QtQuick
import Vermilion
import "../components"

ListView {
  id: root
  boundsMovement: Flickable.StopAtBounds
  clip: true
  property int num
  property var selectedIds: []
  signal itemToggled(string id);

  delegate: CheckboxRow {
    id: plItem

    required property int index
    required property Playlist playlist

    color: "transparent"
    height: root.num ? index < root.num ? 64 : 0 : 64
    selected: root.selectedIds.includes(playlist.id)
    text: plItem.playlist.name
    width: ListView.view.width

    checkBoxBackground: RoundedImage {
      source: plItem.playlist.artUrl
    }

    onClicked: {
      root.itemToggled(plItem.playlist.id)
    }
  }
}
