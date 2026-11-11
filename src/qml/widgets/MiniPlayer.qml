pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vermilion
import "../components"

SwipeDelegate {
  id: root

  property string artUrl
  property var artists
  property color color
  property real contentScale: 1
  property int duration
  property real iconOpacity: 1
  property real imageOpacity: 1
  property bool isBuffering: Player.playerState === PlayerState.Buffering
  property bool isPlaying: Player.playerState === PlayerState.Playing
  property color loadingColor: Qt.lighter(palette.alternateBase, 2)
  property int position
  property bool progressIndicatiorVisible: true
  property real progressIndicatorOpacity: 1
  property real textOpacity: 1
  property string title
  property real yOffset: 0

  focus: true
  padding: 0
  z: 10

  background: Rectangle {
    color: root.color
  }
  contentItem: ColumnLayout {
    id: mainLayout

    scale: root.contentScale
    spacing: 0

    RowLayout {
      id: playerStateContainer

      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.margins: 12
      Layout.maximumWidth: root.width
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

        Rectangle {
          anchors.fill: parent
          color: root.loadingColor
          radius: parent.cornerRadius / 2
          visible: root.isBuffering
        }
      }
      ColumnLayout {
        Layout.fillWidth: true
        clip: true
        opacity: root.textOpacity
        spacing: 0

        Text {
          id: titleText

          Layout.fillWidth: true
          color: palette.text
          elide: Text.ElideRight
          font.pixelSize: Design.fontSize.base
          text: root.isBuffering ? "" : root.title

          Rectangle {
            anchors.fill: parent
            anchors.bottomMargin: 2
            anchors.topMargin: 2
            color: root.loadingColor
            visible: root.isBuffering
            width: 196
          }
        }
        ArtistsText {
          id: artistsText

          Layout.fillWidth: true
          artists: root.isBuffering ? null : root.artists
          fontSize: Design.fontSize.small
          textColor: Qt.darker(palette.text, 2)

          Rectangle {
            anchors.fill: parent
            anchors.bottomMargin: 2
            anchors.topMargin: 2
            color: root.loadingColor
            visible: root.isBuffering
            width: 196 * 2
          }
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
        color: root.isBuffering ? root.loadingColor : "white"

        transform: Translate {
          y: root.yOffset
        }

        onClicked: {
          if (root.isPlaying) {
            Player.pause();
          } else {
            Player.resume();
          }
        }
      }
    }
  }
  swipe.left: Item {
    clip: true
    height: root.height
    width: root.width * 0.2

    Rectangle {
      anchors.left: parent.left
      color: palette.alternateBase
      height: parent.height
      width: Math.max(0, root.swipe.position * root.width)
    }
  }
  swipe.right: Item {
    anchors.right: parent.right
    clip: true
    height: root.height
    width: root.width * 0.2

    Rectangle {
      anchors.right: parent.right
      color: palette.alternateBase
      height: parent.height
      width: Math.max(0, -root.swipe.position * root.width)
    }
  }

  onClicked: {
    console.log("Main content clicked");
  }
  swipe.onOpened: {
    if (swipe.position > 0) {
      Player.prev();
    } else if (swipe.position < 0) {
      Player.next();
    }

    swipe.close();
  }

  Rectangle {
    id: progressInicator

    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    color: Qt.darker(palette.text, 2)
    height: 2
    opacity: root.progressIndicatorOpacity
    z: 15

    Rectangle {
      anchors.bottom: parent.bottom
      anchors.left: parent.left
      anchors.top: parent.top
      color: palette.accent
      width: root.duration > 0 ? (root.position / root.duration) * parent.width : 0
    }
  }
}
