pragma ComponentBehavior: Bound
import QtQml

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Vermilion
import "./components"

import "./widgets"

Item {
  id: rootContent

  property int activeTabIndex: 0
  property var stack: stackRepeater.itemAt(activeTabIndex)

  Component.onCompleted: {
    let history = HotReloader.savedStackHistory;
    let openPlayer = false;

    if (history && history.length > 0) {
      for (let i = 0; i < stackRepeater.count; i++) {
        let sv = stackRepeater.itemAt(i) as StackView;
        if (sv)
          sv.clear(StackView.Immediate);
      }

      for (let i = 0; i < history.length; i++) {
        if (history[i].isPlayerPageActive !== undefined) {
          if (history[i].isPlayerPageActive) {
            openPlayer = true;
          }
          continue;
        }
        if (history[i].currentIndex !== undefined) {
          rootContent.activeTabIndex = history[i].currentIndex;
          continue;
        }

        let sv = stackRepeater.itemAt(history[i].index) as StackView;
        if (sv) {
          sv.pushItem(history[i].url, {
            pageArgs: history[i].args
          } || {}, StackView.Immediate);
        }
      }

      if (openPlayer) {
        playerFull.openPlayer();
      }
      HotReloader.savedStackHistory = [];
      console.log("History restored.");
    }
  }
  onActiveTabIndexChanged: {
    if (navigationLoader.item && "currentIndex" in navigationLoader.item) {
      let navBar = navigationLoader.item;
      if (navBar.currentIndex !== activeTabIndex) {
        navBar.currentIndex = activeTabIndex;
      }
    }
  }

  Connections {
    function onCurrentIndexChanged() {
      if (navigationLoader.item && "currentIndex" in navigationLoader.item) {
        let navBar = navigationLoader.item;
        if (rootContent.activeTabIndex !== navBar.currentIndex) {
          rootContent.activeTabIndex = navBar.currentIndex;
        }
      }
    }

    target: navigationLoader.item
  }
  StateGroup {
    id: layoutState

    state: rootContent.width < 800 ? "mobile" : "desktop"

    states: [
      State {
        name: "mobile"
      },
      State {
        name: "desktop"
      }
    ]
  }
  GridLayout {
    anchors.fill: rootContent
    columnSpacing: 0
    columns: layoutState.state === "mobile" ? 1 : 2
    rowSpacing: 0
    rows: layoutState.state === "mobile" ? 2 : 1

    Loader {
      id: navigationLoader

      Layout.column: 0
      Layout.fillHeight: layoutState.state === "desktop"
      Layout.fillWidth: layoutState.state === "mobile"
      Layout.preferredHeight: layoutState.state === "mobile" ? 68 : -1
      Layout.preferredWidth: layoutState.state === "desktop" ? 72 : -1
      Layout.row: layoutState.state === "mobile" ? 1 : 0
      source: layoutState.state === "mobile" ? "widgets/BottomBar.qml" : "widgets/SideBar.qml"

      Behavior on Layout.preferredHeight {
        NumberAnimation {
          duration: 300
          easing.type: Easing.InOutQuad
        }
      }
    }
    Item {
      id: contentContainer

      Layout.column: layoutState.state === "mobile" ? 0 : 1
      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.row: 0

      StackLayout {
        id: mainContentLayout

        anchors.fill: parent
        clip: true
        currentIndex: rootContent.activeTabIndex

        Repeater {
          id: stackRepeater

          model: ["pages/Home.qml", "pages/Search.qml", "pages/Library.qml", "pages/Settings.qml"]

          StackView {
            id: page

            required property string modelData

            initialItem: Loader {
              anchors.fill: parent
              source: Qt.resolvedUrl(page.modelData)
            }
          }
        }
      }
      MiniPlayer {
        id: miniPlayer

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        TapHandler {
          id: tapHandler

          gesturePolicy: TapHandler.WithinBounds

          onTapped: {
            console.log("Open Player page.");
            playerFull.openPlayer();
          }
        }
      }
      Rectangle {
        id: playerFull

        function openPlayer() {
          anchors.top = parent.top;
          anchors.bottom = parent.bottom;
          anchors.left = parent.left;
          anchors.right = parent.right;
          height = parent.height;
          visible = true;
          navigationLoader.visible = false;
          navigationLoader.Layout.preferredHeight = 0;
          miniPlayer.visible = false;
        }

        color: palette.window
        visible: false

        NumberAnimation {
          id: shrinkAnimation

          duration: 300
          easing.type: Easing.InOutQuad
          property: "height"
          target: playerFull
          to: 80

          onFinished: {
            playerFull.visible = false;
            navigationLoader.visible = true;
            miniPlayer.visible = true;
          }
          onStarted: {
            playerFull.anchors.top = undefined;
          }
        }
        ColumnLayout {
          anchors.fill: parent
          anchors.margins: 32
          spacing: 32

          RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.fillWidth: true
            Layout.maximumWidth: 1920

            IconButton {
              Layout.alignment: Qt.AlignLeft
              Layout.leftMargin: -8
              Layout.preferredHeight: 40
              Layout.preferredWidth: 40
              hasState: false
              source: Qt.resolvedUrl("../../assets/icons/caretDown.svg")

              onClicked: {
                shrinkAnimation.start();
                navigationLoader.Layout.preferredHeight = layoutState.state === "mobile" ? 68 : -1;
              }
            }
            Item {
              id: playlistText

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
              source: Qt.resolvedUrl("../../assets/icons/hamburger.svg")

              onClicked: {
                console.log("Three dots clicked");
              }
            }
          }
          RoundedImage {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.bottomMargin: 16
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: 1920
            Layout.topMargin: 16
            cornerRadius: 24
            source: Player.currentTrack.thumbnailUrl
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
                id: title

                Layout.fillWidth: true
                color: palette.text
                elide: Text.ElideRight
                font.bold: true
                font.pixelSize: 24
                text: Player.currentTrack.title
              }
              Text {
                id: artist

                Layout.fillWidth: true
                color: Qt.darker(palette.text, 2.0)
                elide: Text.ElideRight
                font.pixelSize: 16
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
              id: lyricsButton

              Layout.preferredHeight: 48
              Layout.preferredWidth: 48
              Layout.rightMargin: -8
              hasState: false
              source: Qt.resolvedUrl("../../assets/icons/lyrics.svg")

              onClicked: {
                console.log("Lyrics Pressed.");
              }
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
              handle: null
              to: Player.duration
              value: Player.position

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

                if (hours > 0) {
                  let paddedHours = String(hours).padStart(2, '0');
                  return paddedHours + ":" + paddedMinutes + ":" + paddedSeconds;
                }

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
                text: Player.currentTrack.duration
              }
            }
          }
          RowLayout {
            id: playbackControls

            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.maximumWidth: 1920
            Layout.topMargin: -32

            IconButton {
              id: shuffle

              Layout.alignment: Qt.AlignLeft
              Layout.leftMargin: -8
              Layout.preferredHeight: 48
              Layout.preferredWidth: 48
              hasState: false
              source: Qt.resolvedUrl("../../assets/icons/shuffle.svg")

              onClicked: {
                console.log("Shuffle Pressed");
              }
            }
            RowLayout {
              Layout.alignment: Qt.AlignCenter
              Layout.fillWidth: true
              Layout.maximumWidth: 1920 / 4
              spacing: 8

              Item {
                id: shufflePrevSpacer

                Layout.fillWidth: true
              }
              IconButton {
                id: previous

                Layout.alignment: Qt.AlignRight
                Layout.preferredHeight: 48
                Layout.preferredWidth: 48
                hasState: false
                source: Qt.resolvedUrl("../../assets/icons/previous.svg")

                onClicked: {
                  Player.prev();
                }
              }
              IconButton {
                id: playPauseIcon

                Layout.alignment: Qt.AlignCenter
                Layout.preferredHeight: 96
                Layout.preferredWidth: 96
                hasState: false
                source: Qt.resolvedUrl("../../assets/icons/play-circle.svg")

                states: [
                  State {
                    name: "playing"
                    when: Player.playerState === PlayerState.Playing

                    PropertyChanges {
                      playPauseIcon {
                        source: Qt.resolvedUrl("../../assets/icons/pause-circle.svg")
                      }
                    }
                  },
                  State {
                    name: "paused"
                    when: Player.playerState === PlayerState.Paused

                    PropertyChanges {
                      playPauseIcon {
                        source: Qt.resolvedUrl("../../assets/icons/play-circle.svg")
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
              IconButton {
                id: next

                Layout.alignment: Qt.AlignLeft
                Layout.preferredHeight: 48
                Layout.preferredWidth: 48
                hasState: false
                source: Qt.resolvedUrl("../../assets/icons/next.svg")

                onClicked: {
                  Player.next();
                }
              }
              Item {
                id: nextShuffleSpacer

                Layout.fillWidth: true
              }
            }
            IconButton {
              id: loop

              Layout.alignment: Qt.AlignRight
              Layout.preferredHeight: 48
              Layout.preferredWidth: 48
              Layout.rightMargin: -8
              hasState: false
              source: Qt.resolvedUrl("../../assets/icons/loop.svg")

              onClicked: {
                console.log("Loop Pressed");
              }
            }
          }
          IconButton {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.maximumWidth: 1920
            Layout.preferredHeight: 48
            hasState: false
            source: Qt.resolvedUrl("../../assets/icons/caretUp.svg")

            onClicked: {
              console.log("[ACTION] Open current queue");
              playerQueueExpanded.visible = true;
              playerFull.visible = false;
            }
          }
        }
        Item {
          id: playerBottonSpacer

          Layout.fillHeight: true
        }
      }
      ColumnLayout {
        id: playerQueueExpanded

        anchors.fill: parent
        spacing: 0
        visible: false

        MiniPlayer {
          Layout.alignment: Qt.AlignTop
          Layout.fillWidth: true
          Layout.maximumWidth: 1920
        }
        IconButton {
          Layout.fillWidth: true
          Layout.maximumWidth: 1920
          Layout.preferredHeight: 48
          hasState: false
          source: Qt.resolvedUrl("../../assets/icons/caretDown.svg")

          background: Rectangle {
            anchors.fill: parent
            color: palette.window
          }

          onClicked: {
            console.log("[ACTION] Close current queue");
            playerQueueExpanded.visible = false;
            playerFull.visible = true;
          }
        }
        Rectangle {
          Layout.fillHeight: true
          Layout.fillWidth: true
          color: palette.window

          ListView {
            id: playlistView

            anchors.fill: parent
            anchors.margins: 8
            clip: true
            model: Player.playlist

            delegate: SongItem {
              id: songItem

              focus: true
              height: 80
              width: ListView.view ? ListView.view.width : parent.width

              MouseArea {
                anchors.fill: parent

                onClicked: {
                  playlistView.model.playTrackAtIndex(songItem.index);
                }
              }
            }
          }
        }
      }
    }
  }
  Connections {
    function onCurrentIndexChanged() {
      let bottomBar = navigationLoader.item;
      if (bottomBar)
        rootContent.activeTabIndex = bottomBar.currentIndex;
    }

    target: navigationLoader.item
  }
  Connections {
    function onFileChanged() {
      console.log("File changes detected...");
      let historyArray = [];
      historyArray.push({
        "currentIndex": rootContent.activeTabIndex
      });

      for (let i = 0; i < stackRepeater.count; i++) {
        let sv = stackRepeater.itemAt(i) as StackView;
        if (!sv)
          continue;

        for (let j = 0; j < sv.depth; j++) {
          let stackItem = sv.get(j); // Might be a loader OR the loaded component.
          let pageObj = stackItem && stackItem.item ? stackItem.item : stackItem;
          if (pageObj && pageObj.pageUrl) {
            historyArray.push({
              "url": pageObj.pageUrl,
              "args": pageObj.pageArgs || {},
              "index": i
            });
          }
        }
      }

      if (playerFull) {
        historyArray.push({
          "isPlayerPageActive": playerFull.visible
        });
      }
      console.log("Serialized history size:", historyArray.length);
      HotReloader.savedStackHistory = historyArray;
      HotReloader.requestReload();
    }

    target: HotReloader
  }
}
