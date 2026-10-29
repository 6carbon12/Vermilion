pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts

Item {
  id: root

  required property string duration
  required property int index
  property Component rightContent
  property Component subText: Text {
    color: "#B3B3B3"
    elide: Text.ElideRight
    font.pixelSize: 14
    text: root.duration + " • " + root.views
  }
  required property string thumbnailUrl
  required property string title
  required property string views

  RowLayout {
    id: trackLayout

    anchors.fill: parent
    spacing: 16

    Rectangle {
      id: albumCoverContainer

      Layout.preferredHeight: 64
      Layout.preferredWidth: 64
      clip: true
      color: palette.alternateBase

      Image {
        id: albumCover

        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: root.thumbnailUrl
      }
    }
    ColumnLayout {
      id: content

      Layout.alignment: Qt.AlignVCenter
      Layout.fillWidth: true
      spacing: 4

      Text {
        Layout.fillWidth: true
        color: "white"
        elide: Text.ElideRight
        font.pixelSize: 16
        font.weight: Font.Medium
        text: root.title
      }
      Loader {
        Layout.fillWidth: true
        sourceComponent: root.subText
      }
    }
    Loader {
      Layout.alignment: Qt.AlignVCenter
      Layout.preferredHeight: 48
      Layout.preferredWidth: 40
      sourceComponent: root.rightContent
    }
  }
}
