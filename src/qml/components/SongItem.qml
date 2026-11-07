pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vermilion

Item {
  id: root

  required property string duration
  required property int index
  property Component rightContent
  property Component subText: Text {
    color: "#B3B3B3"
    elide: Text.ElideRight
    font.pixelSize: Design.fontSize.base
    text: root.duration + " • " + root.views
  }
  required property string thumbnailUrl
  required property string title
  required property string views

  signal mainContentClicked
  signal rightContentClicked

  RowLayout {
    id: trackLayout

    anchors.fill: parent
    spacing: 16

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: 64
      color: "transparent"

      Rectangle {
        id: albumCoverContainer

        clip: true
        color: palette.alternateBase
        height: parent.height
        width: parent.height

        Image {
          id: albumCover

          anchors.fill: parent
          fillMode: Image.PreserveAspectCrop
          source: root.thumbnailUrl
        }
      }
      Column {
        id: content

        anchors.left: albumCoverContainer.right
        anchors.right: parent.right
        anchors.leftMargin: trackLayout.spacing
        anchors.verticalCenter: parent.verticalCenter
        spacing: 4

        Text {
          anchors.left: parent.left
          anchors.right: parent.right
          color: "white"
          elide: Text.ElideRight
          clip: true
          font.pixelSize: Design.fontSize.base
          font.weight: Font.Medium
          text: root.title
        }
        Loader {
          anchors.left: parent.left
          anchors.right: parent.right
          clip: true
          sourceComponent: root.subText
        }
      }
      MouseArea {
        anchors.fill: parent

        onClicked: {
          root.mainContentClicked();
        }
      }
    }
    Loader {
      id: rightContent
      Layout.alignment: Qt.AlignVCenter
      Layout.preferredHeight: 48
      Layout.preferredWidth: 40
      sourceComponent: root.rightContent

      MouseArea {
        anchors.fill: parent

        onClicked: {
          root.rightContentClicked();
        }
      }
    }
  }
}
