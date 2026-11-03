pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import "../components"

BasePage {
  id: libraryPage

  pageArgs: ({})
  pageUrl: Qt.resolvedUrl("Library.qml")

  Component.onCompleted: {
    if (libraryPage.pageArgs.stackIndex) {
      stack.currentIndex = libraryPage.pageArgs.stackIndex;
    }

    if (libraryPage.pageArgs.openDialog !== undefined) {
      if (libraryPage.pageArgs.openDialog) {
        const playlistLoader = stackRepeater.itemAt(1);

        if (playlistLoader && playlistLoader.item && typeof playlistLoader.item.openDialog === "function") {
          playlistLoader.item.openDialog();
        } else {
          console.warn("Playlists component is not loaded or `openDialog` function not found.");
        }
      }
    }
  }

  ColumnLayout {
    id: mainLayout

    anchors.fill: parent

    RowLayout {
      id: topBar

      Layout.fillWidth: true
      Layout.leftMargin: 4
      Layout.rightMargin: 4

      Text {
        color: palette.accent
        font.bold: true
        font.pixelSize: 24
        text: "Library"
      }
      Item {
        id: topBarMiddleSpacer

        Layout.fillWidth: true
      }
      IconButton {
        Layout.alignment: Qt.AlignRight
        Layout.preferredHeight: 48
        Layout.preferredWidth: 48
        hasState: false
        source: Qt.resolvedUrl("../../../assets/icons/search-inactive.svg")
      }
    }
    RowLayout {
      Layout.fillWidth: true
      Layout.leftMargin: 4
      Layout.rightMargin: 4

      Repeater {
        model: ["Liked", "Playlists", "Artists", "Downloaded"]

        Rectangle {
          id: tabItemWrapper

          required property int index
          required property string modelData

          color: index == stack.currentIndex ? palette.accent : palette.alternateBase
          height: boldMetrics.height + 16
          radius: 4
          width: boldMetrics.advanceWidth(tabItemWrapper.modelData) + 24

          FontMetrics {
            id: boldMetrics

            font.bold: true
            font.pixelSize: 16
          }
          Text {
            id: tabName

            anchors.centerIn: parent
            color: tabItemWrapper.index == stack.currentIndex ? Qt.darker(palette.text, 0.5) : palette.text
            font.bold: tabItemWrapper.index == stack.currentIndex
            font.pixelSize: 16
            text: tabItemWrapper.modelData
          }
          TapHandler {
            onSingleTapped: {
              stack.currentIndex = tabItemWrapper.index;
              libraryPage.pageArgs.stackIndex = stack.currentIndex;
            }
          }
        }
      }
    }
    StackLayout {
      id: stack

      Layout.leftMargin: 4
      Layout.rightMargin: 4

      Repeater {
        id: stackRepeater

        model: ["../widgets/library/Liked.qml", "../widgets/library/Playlists.qml", "../widgets/library/Artists.qml", "../widgets/library/Downloaded.qml"]

        Loader {
          id: contentLoader

          required property var modelData

          Layout.fillHeight: true
          Layout.fillWidth: true
          source: Qt.resolvedUrl(modelData)

          Connections {
            function onDialogStateChanged(state) {
              libraryPage.pageArgs.openDialog = state;
            }

            ignoreUnknownSignals: true
            target: contentLoader.item
          }
        }
      }
    }
  }
}
