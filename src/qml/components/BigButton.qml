pragma ComponentBehavior: Bound

import QtQuick

Rectangle {
  id: root

  property color clickedColor: Qt.lighter(palette.alternateBase, 2)
  property color hoverColor: Qt.lighter(palette.alternateBase, 1.5)
  property alias iconSource: icon.source
  property alias margins: icon.anchors.margins
  property alias size: icon.size

  signal clicked

  color: icon.pressed ? root.clickedColor : icon.hovered ? root.hoverColor : palette.alternateBase
  radius: 8

  Behavior on color {
    ColorAnimation {
      duration: 150
    }
  }

  IconButton {
    id: icon

    anchors.centerIn: parent
    anchors.fill: parent
    hasState: false

    onClicked: {
      root.clicked();
    }
  }
}
