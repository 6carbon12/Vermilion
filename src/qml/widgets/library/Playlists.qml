pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vermilion as Vermilion
import "../../components"
import "../"

Item {
  id: root

  signal dialogStateChanged(state: bool)

  function openDialog() {
    dialog.open();
  }

  ColumnLayout {
    anchors.fill: parent
    spacing: -4

    RowLayout {
      Text {
        color: palette.text
        font.pixelSize: 18
        text: "Sort By:"
      }
      Item {
        Layout.fillWidth: true
      }
      IconButton {
        Layout.preferredHeight: 40
        Layout.preferredWidth: 40
        hasState: false
        source: "qrc:/icons/plus.svg"

        onClicked: {
          root.openDialog();
        }
      }
    }
    GridView {
      id: rootView

      readonly property int columns: Math.max(1, Math.floor(width / targetCellWidth))
      readonly property real targetCellWidth: 160

      Layout.fillHeight: true
      Layout.fillWidth: true
      cellHeight: cellWidth * 1.2
      cellWidth: width / columns
      clip: true
      model: Vermilion.PlaylistsController.model

      delegate: Item {
        id: delegateWrapper

        required property Vermilion.Playlist playlist

        height: rootView.cellHeight
        width: rootView.cellWidth
        Layout.row: playlist.type === Vermilion.Playlist.Liked ? 1 : 0;
        Layout.column: playlist.type === Vermilion.Playlist.Liked ? 1 : 0;

        PlaylistItem {
          anchors.fill: parent
          anchors.margins: 6
          visible: delegateWrapper.playlist.type !== Vermilion.Playlist.Current
          font.pixelSize: 14
          playlist: delegateWrapper.playlist
        }
      }
    }
  }
  PlaylistCreationModal {
    id: dialog
  }
}
