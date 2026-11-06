pragma ComponentBehavior: Bound

import QtQuick
import Vermilion

Rectangle {
  id: root

  property Component checkBoxBackground
  property bool selected: false
  property string text

  signal clicked

  clip: true
  color: "transparent"

  Row {
    id: playlistDetails

    anchors.verticalCenter: parent.verticalCenter
    spacing: 16

    Rectangle {
      color: root.selected ? Qt.alpha("#000000", 0.5) : "transparent"
      height: root.height - parent.spacing
      width: height
      radius: 12

      Loader {
        height: root.height - playlistDetails.spacing
        sourceComponent: root.checkBoxBackground
        width: height
        z: -1
      }
      IconButton {
        anchors.fill: parent
        hasState: false
        size: Qt.size(Design.iconSize.small, Design.iconSize.small)
        source: "qrc:/icons/check.svg"
        visible: root.selected
      }
    }
    Text {
      color: palette.text
      font.pixelSize: Design.fontSize.base - 2
      font.weight: Font.Medium
      height: parent.height
      text: root.text
      verticalAlignment: Text.AlignVCenter
    }
  }
  Rectangle {
    id: bottomBorder

    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    color: Qt.alpha(Qt.lighter(palette.base, 2), 0.3)
    height: 1
  }
  MouseArea {
    anchors.fill: parent

    onClicked: {
      root.clicked();
    }
  }
}
