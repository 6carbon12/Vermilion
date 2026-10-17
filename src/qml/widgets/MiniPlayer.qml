import QtQuick
import QtQuick.Layouts
import Vermilion

Rectangle {
  id: root
  color: palette.alternateBase
  focus: true
  height: 80
  visible: Player.playerState != PlayerState.Initialized
  z: 10

  ColumnLayout {
    id: mainLayout

    anchors.fill: parent

    RowLayout {
      id: playerStateContainer

      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.margins: 12
      spacing: 16

      Image {
        id: trackThumbnail

        Layout.preferredWidth: parent.height
        source: Player.currentTrack.thumbnailUrl
        sourceSize: Qt.size(parent.height, parent.height)
      }
      ColumnLayout {
        Layout.fillWidth: true
        clip: true

        Text {
          id: title

          Layout.fillWidth: true
          color: palette.text
          elide: Text.ElideRight
          font.pixelSize: 14
          text: Player.currentTrack.title
        }
        Text {
          id: artist

          Layout.fillWidth: true
          color: Qt.darker(palette.text, 2.0)
          elide: Text.ElideRight
          text: {
            if (Player.currentTrack.artist !== "") {
              return Player.currentTrack.artist;
            } else {
              return "Arist Unavailable";
            }
          }
        }
      }
      IconButton {
        id: playPauseIcon

        Layout.alignment: Qt.AlignRight
        Layout.preferredHeight: 40
        Layout.preferredWidth: 40
        hasState: false
        source: Qt.resolvedUrl("../../../assets/icons/pause.svg")

        states: [
          State {
            name: "playing"
            when: Player.playerState === PlayerState.Playing

            PropertyChanges {
              playPauseIcon {
                source: Qt.resolvedUrl("../../../assets/icons/pause.svg")
              }
            }
          },
          State {
            name: "paused"
            when: Player.playerState === PlayerState.Paused

            PropertyChanges {
              playPauseIcon {
                source: Qt.resolvedUrl("../../../assets/icons/play.svg")
              }
            }
          }
        ]

        onClicked: {
          if (Player.playerState === PlayerState.Playing) {
            Player.pause();
          } else {
            Player.resume();
          }
        }
      }
    }
    Rectangle {
      id: progressInicator

      Layout.fillWidth: true
      Layout.preferredHeight: 2
      color: Qt.darker(palette.text, 2)

      Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: parent.top
        color: palette.accent
        width: Player.duration > 0 ? (Player.position / Player.duration) * parent.width : 0
      }
    }
  }
}
