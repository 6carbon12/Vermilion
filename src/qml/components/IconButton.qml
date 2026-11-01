pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Button {
  id: iconButton

  property int bottomMargin: 8
  property bool hasState: true
  property bool isActive: false
  property int leftMargin: 0
  property int rightMargin: 0
  property alias size: icon.sourceSize
  required property string source
  property int topMargin: 8

  background: Rectangle {
    color: "transparent"
  }
  contentItem: Image {
    id: icon

    anchors.bottomMargin: iconButton.bottomMargin
    anchors.fill: iconButton
    anchors.leftMargin: iconButton.leftMargin
    anchors.rightMargin: iconButton.rightMargin
    anchors.topMargin: iconButton.topMargin
    fillMode: Image.PreserveAspectFit
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

    layer.effect: MultiEffect {
      brightness: 1.0
      colorization: 1.0
      colorizationColor: iconButton.isActive ? "#9F9FFF" : "#FFFFFF"
    }

    onStatusChanged: {
      if (icon.status == Image.Error) {
        icon.source = iconButton.source;
      }
    }
  }
}
