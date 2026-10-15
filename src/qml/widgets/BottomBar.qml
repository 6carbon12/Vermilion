pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import "../components" as Componenets

Rectangle {
  id: navBar

  property int currentIndex: 0

  Layout.fillHeight: true
  Layout.fillWidth: true
  clip: true
  color: "#1A1A1A"

  RowLayout {
    anchors.fill: navBar
    spacing: 0

    Repeater {
      model: [
        {
          icon: "../../../assets/icons/home",
          target: Qt.resolvedUrl("../pages/Home.qml"),
          text: "Home"
        },
        {
          icon: "../../../assets/icons/search",
          target: Qt.resolvedUrl("../pages/Search.qml"),
          text: "Search"
        },
        {
          icon: "../../../assets/icons/library",
          target: Qt.resolvedUrl("../pages/Library.qml"),
          text: "Library"
        },
        {
          icon: "../../../assets/icons/setting",
          target: Qt.resolvedUrl("../pages/Settings.qml"),
          text: "Settings"
        }
      ]

      Rectangle {
        id: iconTextContainer

        required property int index
        required property var modelData

        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.preferredWidth: 1
        Layout.topMargin: 4
        color: "#1A1A1A"

        ColumnLayout {
          id: iconTextLayout

          anchors.fill: iconTextContainer
          spacing: 0

          Componenets.IconButton {
            id: icon

            Layout.fillWidth: true
            Layout.preferredHeight: 48
            Layout.preferredWidth: 1
            source: iconTextContainer.modelData.icon
            isActive: navBar.currentIndex === iconTextContainer.index

            background: Rectangle {
              anchors.centerIn: icon
              color: icon.isActive ? "#9F9FFF" : "transparent"
              height: icon.height - 4
              opacity: 0.1
              radius: height / 2
              width: Math.max(0, Math.min(icon.width - 32, 48 * 2))
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
            font.pixelSize: 10
            horizontalAlignment: Text.AlignHCenter
            text: iconTextContainer.modelData.text
            verticalAlignment: Text.AlignTop
          }
        }
      }
    }
  }
}
