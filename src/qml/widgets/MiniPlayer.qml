import QtQuick
import QtQuick.Layouts
import Vermilion
import "../components"

Rectangle {
  id: root

  property string artUrl
  property var artists
  property int duration
  property real iconOpacity: 1
  property real imageOpacity: 1
  property bool isPlaying
  property int position
  property bool progressIndicatiorVisible: true
  property real progressIndicatorOpacity: 1
  property real contentScale: 1
  property real textOpacity: 1
  property string title
  property real yOffset: 0

  signal pauseRequested
  signal playRequested

  // 1. Change background to transparent
  color: "transparent"
  focus: true
  z: 10

  ColumnLayout {
    id: mainLayout

    anchors.fill: parent
    scale: root.contentScale

    RowLayout {
      id: playerStateContainer

      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.margins: 12
      Layout.maximumWidth: parent.width
      spacing: 12

      RoundedImage {
        id: trackThumbnail

        Layout.alignment: Qt.AlignHCenter
        Layout.fillHeight: true
        Layout.maximumHeight: playerStateContainer.width - (2 * 12)
        Layout.preferredWidth: height
        cornerRadius: 8
        opacity: root.imageOpacity
        source: root.artUrl
      }
      ColumnLayout {
        Layout.fillWidth: true
        clip: true
        opacity: root.textOpacity
        spacing: 0

        transform: Translate {
          y: root.yOffset
        }

        Text {
          id: title

          Layout.fillWidth: true
          color: palette.text
          elide: Text.ElideRight
          font.pixelSize: Design.fontSize.base
          text: root.title
        }
        ArtistsText {
          id: artists

          Layout.fillWidth: true
          artists: root.artists
          fontSize: Design.fontSize.small
          textColor: Qt.darker(palette.text, 2)
        }
      }
      IconButton {
        id: playPauseIcon

        Layout.alignment: Qt.AlignRight
        Layout.preferredHeight: 40
        Layout.preferredWidth: 40
        hasState: false
        opacity: root.iconOpacity
        size: Qt.size(Design.iconSize.small, Design.iconSize.small)
        source: root.isPlaying ? "qrc:/icons/pause.svg" : "qrc:/icons/play.svg"

        transform: Translate {
          y: root.yOffset
        }

        onClicked: {
          if (root.isPlaying) {
            root.pauseRequested();
          } else {
            root.playRequested();
          }
        }
      }
    }
    Rectangle {
      id: progressInicator

      Layout.fillWidth: true
      Layout.preferredHeight: 2
      color: Qt.darker(palette.text, 2)

      // Use opacity instead of visible so the layout doesn't jump 2 pixels when sliding starts
      opacity: root.progressIndicatorOpacity
      // visible: opacity > 0

      Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: parent.top
        color: palette.accent
        width: root.duration > 0 ? (root.position / root.duration) * parent.width : 0
      }
    }
  }
}
