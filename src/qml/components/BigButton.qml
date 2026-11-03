pragma ComponentBehavior: Bound

import QtQuick

Rectangle {
  id: root
  color: palette.alternateBase
  radius: 8
  property alias iconSource: icon.source
  property alias margins: icon.anchors.margins
  property alias size: icon.size
  signal clicked();

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
