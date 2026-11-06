import QtQuick

Rectangle {
  id: root

  property alias iconSource: icon.source
  property alias margins: icon.anchors.margins

  signal clicked

  color: palette.alternateBase
  radius: height / 2

  IconButton {
    id: icon

    anchors.fill: parent
    anchors.margins: 8
    hasState: false
    size: Qt.size(height, width)
    source: "qrc:/icons/download.svg"

    onClicked: {
      root.clicked();
    }
  }
}
