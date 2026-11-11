pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import Vermilion

Button {
  id: iconButton

  property color color: palette.text
  property color activeColor: palette.accent
  property bool hasState: true
  property bool isActive: false
  property size size: Qt.size(Design.iconSize.base, Design.iconSize.base)
  required property string source

  background: Rectangle {
    color: "transparent"
  }
  contentItem: Item {
    Image {
      id: icon

      anchors.centerIn: parent
      fillMode: Image.PreserveAspectFit
      height: iconButton.size.height
      layer.enabled: true
      source: {
        if (!iconButton.hasState) {
          return iconButton.source;
        } else {
          let suffix = "";
          if (iconButton.isActive) {
            suffix = "-active.svg";
          } else {
            suffix = "-inactive.svg";
          }
          return iconButton.source + suffix;
        }
      }
      sourceSize: Qt.size(width, height)
      width: iconButton.size.width

      layer.effect: MultiEffect {
        brightness: 1.0
        colorization: 1.0
        colorizationColor: iconButton.isActive ? iconButton.activeColor : iconButton.color

        Behavior on colorizationColor {
          ColorAnimation {
            duration: 150
          }
        }
      }

      onStatusChanged: {
        if (icon.status == Image.Error) {
          icon.source = iconButton.source;
        }
      }
    }
  }
}
