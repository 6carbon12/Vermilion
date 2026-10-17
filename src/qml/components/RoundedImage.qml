import QtQuick
import QtQuick.Effects

Rectangle {
  id: albumCoverContainer

  color: "transparent"
  property alias source: albumCover.source
  property alias cornerRadius: maskRect.radius

  Item {
    id: imageFrame

    anchors.centerIn: parent
    height: Math.min(parent.height, parent.width)
    width: height

    Image {
      id: albumCover

      anchors.fill: parent
      fillMode: Image.PreserveAspectCrop
      visible: false
    }
    Rectangle {
      id: maskRect

      anchors.fill: parent
      color: "black"
      layer.enabled: true
      radius: 12
      visible: false
    }
    MultiEffect {
      anchors.fill: parent
      maskEnabled: true
      maskSource: maskRect
      source: albumCover
    }
  }
}
