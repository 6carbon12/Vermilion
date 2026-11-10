pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vermilion

SwipeDelegate {
  id: root

  required property string duration
  required property int index
  property color leftSwipeColor
  property string leftSwipeIcon: "qrc:/icons/add-to-playlist.svg"
  property Component rightContent
  property color rightSwipeColor
  property string rightSwipeIcon: "qrc:/icons/play-next.svg"
  property Component subText: Text {
    color: "#B3B3B3"
    elide: Text.ElideRight
    font.pixelSize: Design.fontSize.base
    text: root.duration + " • " + root.views
  }
  required property string thumbnailUrl
  required property string title
  required property string views

  signal leftSwipeActionTriggered
  signal mainContentClicked
  signal rightContentClicked
  signal rightSwipeActionTriggered

  padding: 0

  background: Rectangle {
    color: palette.window
  }
  contentItem: RowLayout {
    id: trackLayout

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
        anchors.leftMargin: trackLayout.spacing
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 4

        Text {
          anchors.left: parent.left
          anchors.right: parent.right
          clip: true
          color: "white"
          elide: Text.ElideRight
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
        acceptedButtons: Qt.RightButton
        anchors.fill: parent

        onClicked: root.rightContentClicked()
      }
    }
    Loader {
      id: rightContent

      Layout.alignment: Qt.AlignVCenter
      Layout.preferredHeight: 48
      Layout.preferredWidth: 40
      sourceComponent: root.rightContent

      MouseArea {
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        anchors.fill: parent

        onClicked: {
          root.rightContentClicked();
        }
      }
    }
  }
  swipe.left: Rectangle {
    anchors.left: parent.left
    clip: true
    color: "dodgerblue"
    height: root.height
    width: root.width * 0.25

    Item {
      anchors.left: parent.left
      height: parent.height
      width: Math.max(0, root.swipe.position * parent.width)

      Image {
        property real exposedProgress: parent.width / (parent.width * 0.25)

        anchors.centerIn: parent
        height: Design.iconSize.small
        opacity: scale > 0 ? 1 : 0
        scale: Math.min(1, Math.max(0, exposedProgress))
        source: root.rightSwipeIcon
        sourceSize: Qt.size(width, height)
        width: Design.iconSize.small
      }
    }
  }
  swipe.right: Rectangle {
    anchors.right: parent.right
    clip: true
    color: "dodgerblue"
    height: root.height
    width: root.width * 0.25

    Item {
      anchors.right: parent.right
      height: parent.height
      width: Math.max(0, -root.swipe.position * parent.width)

      Image {
        property real exposedProgress: parent.width / (parent.width * 0.25)

        anchors.centerIn: parent
        height: Design.iconSize.small
        opacity: scale > 0 ? 1 : 0
        scale: Math.min(1, Math.max(0, exposedProgress))
        source: root.leftSwipeIcon
        sourceSize: Qt.size(width, height)
        width: Design.iconSize.small
      }
    }
  }

  onClicked: root.mainContentClicked()
  onPressAndHold: root.rightContentClicked()
  swipe.onOpened: {
    if (swipe.position > 0) {
      root.rightSwipeActionTriggered();
    } else if (swipe.position < 0) {
      root.leftSwipeActionTriggered();
    }
    swipe.close();
  }
}
