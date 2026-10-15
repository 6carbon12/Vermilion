pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Button {
  id: iconButton

  required property string source
  property bool isActive: false
  property bool hasState: true
  property alias size: icon.sourceSize

  background: Rectangle {
    color: "#1A1A1A"
  }
  contentItem: Image {
    id: icon

    anchors.bottomMargin: 8
    anchors.fill: iconButton
    anchors.topMargin: 8
    fillMode: Image.PreserveAspectFit
    layer.enabled: true
    // TODO: Check if modified sources exit, if not then load the `iconPath` as-is.
    source: {
      if (!iconButton.hasState) {
        return iconButton.source
      } else {
        let suffix = "";
        if (iconButton.isActive) {
          suffix = "-active.svg"
        } else {
          suffix = "-inactive.svg"
        }
        return iconButton.source + suffix
      }
    }

    onStatusChanged: {
      if (icon.status == Image.Error) {
        icon.source = iconButton.source
      }
    }

    layer.effect: MultiEffect {
      brightness: 1.0
      colorization: 1.0
      colorizationColor: iconButton.isActive ? "#9F9FFF" : "#FFFFFF"
    }
  }
}
