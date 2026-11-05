import QtQuick
import QtQuick.Controls

TextField {
  id: input

  EnterKey.type: Qt.EnterKeySearch
  font.pixelSize: 14
  color: palette.text
  leftPadding: height / 2
  rightPadding: height

  background: Rectangle {
    anchors.fill: parent
    color: palette.window
    radius: parent.height / 2
  }
}
