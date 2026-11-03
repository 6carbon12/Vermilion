import QtQuick
import QtQuick.Controls

TextField {
  id: input

  EnterKey.type: Qt.EnterKeySearch
  font.pixelSize: 14
  color: palette.text
  leftPadding: 12
  rightPadding: 12

  background: Rectangle {
    anchors.fill: parent
    color: palette.window
    radius: parent.height / 4
  }
}
