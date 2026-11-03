pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Vermilion

RowLayout {
  id: root

  property color color: palette.text
  property string icon
  property alias iconSize: icon.size
  property string text

  spacing: 0

  IconButton {
    id: icon

    Layout.alignment: Qt.AlignLeft
    color: root.color
    hasState: false
    source: root.icon
    Layout.preferredHeight: root.iconSize.height
    Layout.preferredWidth: root.iconSize.width
  }
  Text {
    Layout.alignment: Qt.AlignHCenter
    color: root.color
    font.pixelSize: Design.fontSize.base
    text: root.text
  }
}
