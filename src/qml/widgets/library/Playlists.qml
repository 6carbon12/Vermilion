pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vermilion as Vermilion
import "../../components"
import "../"

Item {
  id: root

  signal dialogStateChanged(state: bool)

  function openDialog() {
    dialog.open();
  }

  ColumnLayout {
    anchors.fill: parent
    spacing: -4

    RowLayout {
      Text {
        color: palette.text
        font.pixelSize: 18
        text: "Sort By:"
      }
      ComboBox {
        id: sortPreference

        Layout.preferredHeight: implicitHeight
        Layout.preferredWidth: implicitWidth
        bottomPadding: 8
        font.pixelSize: 14
        leftPadding: 8
        model: ["A-Z", "Created"]
        rightPadding: 48
        topPadding: 8

        background: Rectangle {
          border.color: Qt.lighter(palette.alternateBase, 2)
          border.width: 1
          color: palette.window
          radius: 4
        }
        contentItem: Text {
          color: palette.text
          elide: Text.ElideRight
          font: sortPreference.font
          text: sortPreference.displayText
          verticalAlignment: Text.AlignVCenter
        }
        delegate: Rectangle {
          id: sortItemDelegate

          required property int index
          readonly property bool isHighlighted: view.mouseHoverIndex === index || (view.mouseHoverIndex === -1 && sortPreference.highlightedIndex === index)
          required property string modelData
          readonly property ListView view: ListView.view

          color: isHighlighted ? palette.accent : "transparent"
          height: 32
          radius: 4
          width: sortPreference.width

          Text {
            anchors.fill: parent
            anchors.leftMargin: 6
            anchors.rightMargin: 6
            color: isHighlighted ? palette.window : palette.text
            font: sortPreference.font
            text: sortItemDelegate.modelData
            verticalAlignment: Text.AlignVCenter
          }
          MouseArea {
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
              sortPreference.currentIndex = sortItemDelegate.index;
              sortPreference.activated(sortItemDelegate.index);
              sortPreference.popup.close();
            }

            // Tell the ListView exactly which item the mouse is on
            onEntered: sortItemDelegate.view.mouseHoverIndex = sortItemDelegate.index
            onExited: {
              if (sortItemDelegate.view.mouseHoverIndex === sortItemDelegate.index) {
                sortItemDelegate.view.mouseHoverIndex = -1;
              }
            }
          }
        }
        indicator: Canvas {
          id: canvas

          contextType: "2d"
          height: 6
          width: 10
          x: sortPreference.width - width - 12
          y: sortPreference.topPadding + (sortPreference.availableHeight - height) / 2

          onPaint: {
            var ctx = canvas.getContext("2d");
            ctx.reset();
            ctx.moveTo(0, 0);
            ctx.lineTo(width, 0);
            ctx.lineTo(width / 2, height);
            ctx.closePath();
            ctx.fillStyle = Qt.lighter(palette.alternateBase, 2);
            ctx.fill();
          }

          Connections {
            function onPressedChanged() {
              canvas.requestPaint();
            }

            target: sortPreference
          }
        }
        popup: Popup {
          implicitHeight: contentItem.implicitHeight + 8
          padding: 4
          width: 75 + 8
          y: sortPreference.height + 4

          background: Rectangle {
            border.color: palette.mid
            border.width: 1
            color: palette.window
            radius: 6
          }
          contentItem: ListView {
            property int mouseHoverIndex: -1

            clip: true
            currentIndex: sortPreference.highlightedIndex
            highlight: null
            implicitHeight: contentHeight
            model: sortPreference.popup.visible ? sortPreference.delegateModel : null

            onVisibleChanged: {
              if (!visible)
                mouseHoverIndex = -1;
            }
          }
        }

        onActivated: {
          console.log("[ACTION] Sort plylists:", sortPreference.currentValue);
        }
      }
      Item {
        Layout.fillWidth: true
      }
      IconButton {
        Layout.preferredHeight: 40
        Layout.preferredWidth: 40
        hasState: false
        source: "qrc:/icons/plus.svg"

        onClicked: {
          root.openDialog();
        }
      }
    }
    GridView {
      id: rootView

      readonly property int columns: Math.max(1, Math.floor(width / targetCellWidth))
      readonly property real targetCellWidth: 160

      Layout.fillHeight: true
      Layout.fillWidth: true
      cellHeight: cellWidth * 1.2
      cellWidth: width / columns
      clip: true
      model: Vermilion.PlaylistsController.model

      delegate: Item {
        id: delegateWrapper

        required property Vermilion.Playlist playlist

        height: rootView.cellHeight
        width: rootView.cellWidth

        PlaylistItem {
          anchors.fill: parent
          anchors.margins: 6
          font.pixelSize: 14
          playlist: delegateWrapper.playlist
        }
      }
    }
  }
  PlaylistCreationModal {
    id: dialog
  }
}
