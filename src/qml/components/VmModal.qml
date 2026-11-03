import QtQuick
import QtQuick.Controls

Popup {
  modal: true
  padding: 0
  parent: Overlay.overlay
  anchors.centerIn: parent
  Overlay.modal: Rectangle {
    color: Qt.alpha(palette.window, 0.5)
  }
  background: Rectangle {
    border.color: Qt.lighter(palette.alternateBase, 2)
    border.width: 1
    color: palette.alternateBase
    radius: 8
  }
}
