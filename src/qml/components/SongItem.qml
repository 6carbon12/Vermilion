import QtQuick
import QtQuick.Layouts

Item {
  id: root

  required property int index
  required property string duration
  required property string thumbnailUrl
  required property string title
  required property string views
  property Component rightContent

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
      Text {
        Layout.fillWidth: true
        color: "#B3B3B3"
        elide: Text.ElideRight
        font.pixelSize: 14
        text: root.duration + " • " + root.views
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
