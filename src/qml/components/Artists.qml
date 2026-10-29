pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

Flow {
  id: root

  property var artists: []
  property int artistsSize: root.artists ? root.artists.length : 0
  property int fontSize: 14
  property bool interactive: false
  property color textColor: palette.text

  spacing: 0

  Repeater {
    model: root.artistsSize

    RowLayout {
      id: artistLayout

      readonly property var currentArtist: root.artists[artistLayout.index]
      required property int index

      spacing: 0

      Text {
        id: nameText

        color: root.textColor
        font.pixelSize: root.fontSize
        text: artistLayout.currentArtist ? (artistLayout.currentArtist.name || "") : ""

        MouseArea {
          id: mouseArea

          anchors.fill: parent
          cursorShape: root.interactive ? Qt.PointingHandCursor : Qt.ArrowCursor
          hoverEnabled: root.interactive

          onClicked: {
            if (root.interactive && artistLayout.currentArtist) {
              console.log("[ACTION] Open artist", artistLayout.currentArtist.id);
            }
          }
        }
      }
      Text {
        color: root.textColor
        font.pixelSize: root.fontSize
        text: artistLayout.index === root.artistsSize - 2 ? " & " : ", "
        visible: artistLayout.index < root.artistsSize - 1
      }
    }
  }
}
