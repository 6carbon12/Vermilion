pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Vermilion
import "../components" as Componenets

Rectangle {
  id: navBar

  property int currentIndex: 0
  property var navItems: [
    {
      icon: "qrc:/icons/home",
      target: Qt.resolvedUrl("../pages/Home.qml"),
      text: "Home"
    },
    {
      icon: "qrc:/icons/search",
      target: Qt.resolvedUrl("../pages/Search.qml"),
      text: "Search"
    },
    {
      icon: "qrc:/icons/library",
      target: Qt.resolvedUrl("../pages/Library.qml"),
      text: "Library"
    },
    {
      icon: "qrc:/icons/setting",
      target: Qt.resolvedUrl("../pages/Settings.qml"),
      text: "Settings"
    }
  ]
  property int transitionTime: 250

  Layout.fillHeight: true
  Layout.fillWidth: true
  clip: true
  color: palette.alternateBase

  Rectangle {
    id: slidingIndicator

    property real segmentWidth: navBar.width / navBar.navItems.length

    color: "#9F9FFF"
    height: Design.iconSize.base + 8
    opacity: 0.1
    radius: height / 2
    width: Design.iconSize.base * 2.5
    x: (navBar.currentIndex * segmentWidth) + ((segmentWidth - width) / 2)
    y: 6

    Behavior on x {
      NumberAnimation {
        duration: navBar.transitionTime
      }
    }
  }
  RowLayout {
    anchors.fill: navBar
    spacing: 0

    Repeater {
      model: navBar.navItems

      Rectangle {
        id: iconTextContainer

        required property int index
        required property var modelData

        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.preferredWidth: 1
        Layout.topMargin: 4
        color: "transparent"

        ColumnLayout {
          id: iconTextLayout

          anchors.fill: iconTextContainer
          spacing: 0

          Componenets.IconButton {
            id: icon

            Layout.fillWidth: true
            Layout.preferredHeight: Design.iconSize.base
            Layout.preferredWidth: 1
            isActive: navBar.currentIndex === iconTextContainer.index
            source: iconTextContainer.modelData.icon

            background: Item {
            }

            onClicked: {
              navBar.currentIndex = iconTextContainer.index;
            }
          }
          Text {
            Layout.fillWidth: true
            Layout.preferredHeight: 16
            Layout.preferredWidth: 1
            color: navBar.currentIndex === iconTextContainer.index ? "#9F9FFF" : "#FFFFFF"
            font.pixelSize: Design.fontSize.xSmall
            horizontalAlignment: Text.AlignHCenter
            text: iconTextContainer.modelData.text
            verticalAlignment: Text.AlignTop

            Behavior on color {
              ColorAnimation {
                duration: navBar.transitionTime
              }
            }
          }
        }
      }
    }
  }
}
