pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import Vermilion
import "../components"

RowLayout {
  id: root

  required property Playlist pt

  Text {
    Layout.fillHeight: true
    color: palette.text
    font.bold: true
    font.pixelSize: 16
    text: "Sort By:"
    verticalAlignment: Text.AlignVCenter
  }
  VmDropdown {
    id: sortDropdown

    property bool isReversed: root.pt.isReverse

    function sortTypeToString(type) {
      switch (type) {
      case Playlist.Title:
        return "Title";
      case Playlist.Artist:
        return "Artist";
      case Playlist.Duration:
        return "Duration";
      case Playlist.DateAdded:
        return "Date added"; // Lowercase 'a'
      case Playlist.Custom:
        return "Custom";
      default:
        return "";
      }
    }
    function stringToSortType(str) {
      switch (str) {
      case "Title":
        return Playlist.Title;
      case "Artist":
        return Playlist.Artist;
      case "Duration":
        return Playlist.Duration;
      case "Date added":
        return Playlist.DateAdded;
      case "Custom":
        return Playlist.Custom;
      default:
        return Playlist.DateAdded;
      }
    }

    Layout.alignment: Qt.AlignVCenter
    Layout.fillHeight: true
    Layout.fillWidth: true
    options: ["Title", "Artist", "Duration", "Date added", "Custom"]
    selected: root.pt ? sortTypeToString(root.pt.sortBy) : false

    caret: Image {
      layer.enabled: true
      rotation: root.pt.isReverse ? 180 : 0
      source: Qt.resolvedUrl("../../../assets/icons/caret-down-fill.svg")
      sourceSize: Qt.size(height, width)
      transformOrigin: Item.Center

      layer.effect: MultiEffect {
        brightness: 1.0
        colorization: 1.0
        colorizationColor: palette.text
      }
    }
    delegate: Rectangle {
      id: sortDropdownDelegate

      required property int index
      readonly property bool isFirstItem: sortDropdownDelegate.index === 0
      readonly property bool isLastItem: sortDropdownDelegate.index === (sortDropdown.options.length - 1)
      property bool isSelected: sortDropdown.selected ? sortDropdownDelegate.modelData === sortDropdown.selected : false
      required property string modelData
      readonly property real targetRadius: sortDropdown.radius || 0

      bottomLeftRadius: sortDropdownDelegate.isLastItem ? sortDropdownDelegate.targetRadius : 0
      bottomRightRadius: sortDropdownDelegate.isLastItem ? sortDropdownDelegate.targetRadius : 0
      color: sortDropdownDelegate.isSelected ? Qt.lighter(palette.alternateBase, 2) : "transparent"
      height: sortDropdown.itemHeight
      topLeftRadius: sortDropdownDelegate.isFirstItem ? sortDropdownDelegate.targetRadius : 0
      topRightRadius: sortDropdownDelegate.isFirstItem ? sortDropdownDelegate.targetRadius : 0
      width: sortDropdown.itemWidth

      RowLayout {
        anchors.fill: parent
        anchors.margins: 16

        Text {
          Layout.preferredHeight: parent.height
          color: palette.text
          font.pixelSize: Design.fontSize.base
          font.weight: Font.DemiBold
          text: sortDropdownDelegate.modelData
          verticalAlignment: Text.AlignVCenter
        }
        Item {
          id: dropdownItemMiddleSpacer

          Layout.fillWidth: true
        }
        Image {
          id: selectedItemRev

          Layout.preferredHeight: parent.height - 8
          Layout.preferredWidth: parent.height - 8
          layer.enabled: true
          rotation: root.pt.isReverse ? 180 : 0
          source: Qt.resolvedUrl("../../../assets/icons/caret-down-fill.svg")
          sourceSize: Qt.size(height, width)
          transformOrigin: Item.Center
          visible: sortDropdownDelegate.isSelected

          layer.effect: MultiEffect {
            brightness: 1.0
            colorization: 1.0
            colorizationColor: palette.text
          }
        }
      }
      MouseArea {
        anchors.fill: parent

        onClicked: {
          if (sortDropdown.selected === sortDropdownDelegate.modelData) {
            PlaylistsController.updateSortOrder(root.pt.id, root.pt.sortBy, !root.pt.isReverse);
          } else {
            let sort = sortDropdown.stringToSortType(sortDropdownDelegate.modelData);
            PlaylistsController.updateSortOrder(root.pt.id, sort, root.pt.isReverse);
          }
        }
      }
    }
  }
}
