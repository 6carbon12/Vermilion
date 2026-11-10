// PlayerView.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vermilion

Item {
  id: root

  property real _startHeight: collapsedHeight
  property int collapsedHeight: 80
  property real currentHeight: collapsedHeight
  property real currentVelocityY: 0
  property int expandedHeight: parent ? parent.height : 600
  property real fullProgress: Math.min(1, Math.max(0, (root.progress - 0.1) / 0.9))
  property real lastTime: 0
  property real lastTranslationY: 0
  property real miniProgress: Math.min(1, root.progress / 0.15)
  property real progress: (currentHeight - collapsedHeight) / Math.max(1, expandedHeight - collapsedHeight)
  property real queueProgress: 0.0
  property Track track: Player.currentTrack

  clip: true
  height: currentHeight

  Behavior on currentHeight {
    NumberAnimation {
      duration: 350
      easing.type: Easing.OutQuart
    }
  }
  Behavior on queueProgress {
    NumberAnimation {
      duration: 250
      easing.type: Easing.OutQuart
    }
  }

  Rectangle {
    id: sheetBackground

    anchors.fill: parent
    color: palette.window
  }
  DragHandler {
    id: dragHandler

    target: null
    xAxis.enabled: false
    yAxis.enabled: true

    onActiveChanged: {
      if (active) {
        root._startHeight = root.currentHeight;
        root.lastTranslationY = 0;
        root.lastTime = Date.now();
        root.currentVelocityY = 0;
      } else {
        let currentProgress = (root.currentHeight - root.collapsedHeight) / Math.max(1, root.expandedHeight - root.collapsedHeight);
        let targetExpanded = false;

        if (Math.abs(root.currentVelocityY) > 0.4) {
          targetExpanded = root.currentVelocityY < 0;
        } else {
          targetExpanded = currentProgress > 0.3;
        }

        root.currentHeight = targetExpanded ? root.expandedHeight : root.collapsedHeight;

        if (root.queueProgress > 0.5 && targetExpanded) {
          root.queueProgress = 1.0;
        } else {
          root.queueProgress = 0.0;
        }
      }
    }
    onTranslationChanged: {
      if (active) {
        let now = Date.now();
        let dt = now - root.lastTime;
        if (dt > 0) {
          let dy = translation.y - root.lastTranslationY;
          root.currentVelocityY = dy / dt;
          root.lastTranslationY = translation.y;
          root.lastTime = now;
        }

        let newHeight = root._startHeight - translation.y;
        root.currentHeight = Math.max(root.collapsedHeight, Math.min(root.expandedHeight, newHeight));
      }
    }
  }
  MiniPlayer {
    id: miniPlayer

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    artUrl: root.track.coverArtUrl
    artists: root.track.artists
    color: palette.alternateBase
    contentScale: 1 - (root.miniProgress * 0.15)
    duration: root.track.duration
    height: root.collapsedHeight
    isPlaying: Player.playerState == PlayerState.Playing
    opacity: (1 - root.miniProgress) * (1 - root.queueProgress)
    position: Player.position
    progressIndicatorOpacity: root.progress === 0 ? 1 : 0
    title: root.track.title
    visible: opacity > 0

    MouseArea {
      anchors.fill: parent
      enabled: root.progress === 0

      onClicked: {
        root.queueProgress = 0.0;
        root.currentHeight = root.expandedHeight;
      }
    }
  }
  Item {
    id: playerFull

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    height: root.expandedHeight
    opacity: (1 - root.queueProgress) * (root.progress > 0 ? 1 : 0)
    visible: opacity > 0

    ColumnLayout {
      anchors.fill: parent
      anchors.margins: 32
      spacing: 32

      RowLayout {
        id: topBar

        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        Layout.fillWidth: true
        Layout.maximumWidth: 1920
        opacity: root.fullProgress

        IconButton {
          Layout.alignment: Qt.AlignLeft
          Layout.leftMargin: -8
          Layout.preferredHeight: 40
          Layout.preferredWidth: 40
          hasState: false
          source: "qrc:/icons/caretDown.svg"

          onClicked: {
            root.queueProgress = 0.0;
            root.currentHeight = root.collapsedHeight;
          }
        }
        Item {
          Layout.fillWidth: true
          Layout.preferredHeight: 40

          Text {
            anchors.centerIn: parent
            color: Qt.darker(palette.text, 2)
            elide: Text.ElideRight
            font.pixelSize: 14
            maximumLineCount: 1
            text: "Playing from 'Liked Songs'"
          }
        }
        IconButton {
          Layout.alignment: Qt.AlignRight
          Layout.preferredHeight: 40
          Layout.preferredWidth: 40
          Layout.rightMargin: -8
          hasState: false
          source: "qrc:/icons/hamburger.svg"
        }
      }
      RoundedImage {
        id: fullArt

        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        Layout.bottomMargin: 16
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.maximumWidth: 1920
        Layout.topMargin: 16
        cornerRadius: 24
        opacity: root.fullProgress
        scale: 0.8 + (0.2 * root.fullProgress)
        source: Player.currentTrack.coverArtUrl
      }
      ColumnLayout {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        Layout.fillWidth: true
        opacity: root.fullProgress
        spacing: 32

        transform: Translate {
          y: (1 - root.fullProgress) * 40
        }

        RowLayout {
          id: trackInfo

          Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
          Layout.fillWidth: true
          Layout.maximumWidth: 1920

          ColumnLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.fillWidth: true
            clip: true
            spacing: 0

            Text {
              Layout.fillWidth: true
              color: palette.text
              elide: Text.ElideRight
              font.bold: true
              font.pixelSize: 24
              text: Player.currentTrack.title
            }
            ArtistsText {
              Layout.fillWidth: true
              artists: Player.currentTrack.artists
              fontSize: 16
              textColor: Qt.darker(palette.text, 2)
            }
          }
          IconButton {
            Layout.preferredHeight: 48
            Layout.preferredWidth: 48
            Layout.rightMargin: -8
            hasState: false
            source: "qrc:/icons/heart.svg"
          }
        }
        ColumnLayout {
          id: trackProgress

          Layout.alignment: Qt.AlignHCenter
          Layout.fillWidth: true
          Layout.maximumWidth: 1920

          Slider {
            id: progressBar

            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            from: 0
            to: Player.duration
            value: pressed ? value : Player.position

            background: Rectangle {
              color: Qt.alpha(palette.text, 0.1)
              height: implicitHeight
              implicitHeight: 4
              implicitWidth: 200
              radius: 3
              width: progressBar.availableWidth
              x: progressBar.leftPadding
              y: progressBar.topPadding + progressBar.availableHeight / 2 - height / 2

              Rectangle {
                color: palette.accent
                height: parent.height
                radius: parent.radius
                width: progressBar.visualPosition * parent.width
              }
            }
            handle: Rectangle {
              implicitHeight: 16
              implicitWidth: 16
              radius: implicitHeight / 2
              x: progressBar.position * progressBar.width - (implicitWidth / 2)
            }

            onPressedChanged: {
              if (!pressed) {
                progressBar.background.implicitHeight = 4;
                progressBar.handle.opacity = 1;
                Player.seekTo(progressBar.value);
              } else {
                progressBar.background.implicitHeight = 8;
                progressBar.handle.opacity = 0;
              }
            }
          }
          RowLayout {
            id: progressText

            function formatMillisToDynamicTime(ms) {
              let totalSeconds = Math.floor(ms / 1000);
              let hours = Math.floor(totalSeconds / 3600);
              let minutes = Math.floor((totalSeconds % 3600) / 60);
              let seconds = totalSeconds % 60;
              let paddedMinutes = String(minutes).padStart(2, '0');
              let paddedSeconds = String(seconds).padStart(2, '0');
              if (hours > 0)
                return String(hours).padStart(2, '0') + ":" + paddedMinutes + ":" + paddedSeconds;
              return paddedMinutes + ":" + paddedSeconds;
            }

            Text {
              color: Qt.alpha(palette.text, 0.5)
              font.pixelSize: 12
              text: progressText.formatMillisToDynamicTime(Player.position)
            }
            Item {
              Layout.fillWidth: true
            }
            Text {
              color: Qt.alpha(palette.text, 0.5)
              font.pixelSize: 12
              text: Player.currentTrack.durationString
            }
          }
        }
        RowLayout {
          id: playbackControls

          Layout.alignment: Qt.AlignHCenter
          Layout.fillWidth: true
          Layout.maximumWidth: 1920
          Layout.topMargin: -16

          IconButton {
            Layout.alignment: Qt.AlignLeft
            Layout.leftMargin: -8
            Layout.preferredHeight: 48
            Layout.preferredWidth: 48
            hasState: false
            source: "qrc:/icons/shuffle.svg"
          }
          RowLayout {
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.maximumWidth: 1920 / 4
            spacing: 8

            Item {
              Layout.fillWidth: true
            }
            IconButton {
              Layout.alignment: Qt.AlignRight
              Layout.preferredHeight: 48
              Layout.preferredWidth: 48
              hasState: false
              source: "qrc:/icons/previous.svg"

              onClicked: Player.prev()
            }
            IconButton {
              id: playPauseIcon

              Layout.alignment: Qt.AlignCenter
              Layout.preferredHeight: 96
              Layout.preferredWidth: 96
              hasState: false
              size: Qt.size(Design.iconSize.large, Design.iconSize.large)
              source: "qrc:/icons/play-circle.svg"

              states: [
                State {
                  name: "playing"
                  when: Player.playerState === PlayerState.Playing

                  PropertyChanges {
                    playPauseIcon {
                      source: "qrc:/icons/pause-circle.svg"
                    }
                  }
                },
                State {
                  name: "paused"
                  when: Player.playerState === PlayerState.Paused

                  PropertyChanges {
                    playPauseIcon {
                      source: "qrc:/icons/play-circle.svg"
                    }
                  }
                }
              ]

              onClicked: Player.playerState === PlayerState.Playing ? Player.pause() : Player.resume()
            }
            IconButton {
              Layout.alignment: Qt.AlignLeft
              Layout.preferredHeight: 48
              Layout.preferredWidth: 48
              hasState: false
              source: "qrc:/icons/next.svg"

              onClicked: Player.next()
            }
            Item {
              Layout.fillWidth: true
            }
          }
          IconButton {
            Layout.alignment: Qt.AlignRight
            Layout.preferredHeight: 48
            Layout.preferredWidth: 48
            Layout.rightMargin: -8
            hasState: false
            source: "qrc:/icons/loop.svg"
          }
        }
      }
      Item {
        id: caretContainer

        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        Layout.maximumWidth: 1920
        Layout.preferredHeight: 48

        IconButton {
          id: caretButton

          anchors.fill: parent
          hasState: false
          opacity: root.fullProgress
          source: "qrc:/icons/caretUp.svg"

          onClicked: {
            root.queueProgress = root.queueProgress > 0.5 ? 0.0 : 1.0;
          }
        }
        DragHandler {
          id: caretDragHandler

          property real startQueueProgress: 0

          target: null
          xAxis.enabled: false
          yAxis.enabled: true

          onActiveChanged: {
            if (active) {
              startQueueProgress = root.queueProgress;
            } else {
              root.queueProgress = root.queueProgress > 0.3 ? 1.0 : 0.0;
            }
          }
          onTranslationChanged: {
            if (active) {
              let deltaProgress = -translation.y / 150.0; // 150px full swipe distance
              root.queueProgress = Math.min(1.0, Math.max(0.0, startQueueProgress + deltaProgress));
            }
          }
        }
      }
    }
  }
  ColumnLayout {
    id: playerQueueExpanded

    anchors.fill: parent
    opacity: root.queueProgress
    spacing: 0
    visible: opacity > 0

    MiniPlayer {
      Layout.alignment: Qt.AlignTop
      Layout.fillWidth: true
      Layout.maximumWidth: 1920
      Layout.preferredHeight: 80
      artUrl: root.track.coverArtUrl
      artists: root.track.artists
      color: palette.alternateBase
      duration: root.track.duration
      position: Player.position
      title: root.track.title
    }
    IconButton {
      Layout.fillWidth: true
      Layout.maximumWidth: 1920
      Layout.preferredHeight: 48
      hasState: false
      source: "qrc:/icons/caretDown.svg"

      background: Rectangle {
        anchors.fill: parent
        color: palette.window
      }

      onClicked: {
        root.queueProgress = 0.0;
      }
    }
    Rectangle {
      Layout.fillHeight: true
      Layout.fillWidth: true
      color: palette.window

      SongListView {
        id: playlistView

        anchors.fill: parent
        anchors.margins: 8
        clip: true
        model: PlaylistsController.getTracksFromPlaylist(Player.playlist.id)
      }
    }
  }
}
