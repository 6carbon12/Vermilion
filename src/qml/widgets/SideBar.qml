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

  ColumnLayout {
    anchors.fill: navBar

    Item {
      id: upperSpacer

      Layout.fillHeight: true
    }
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

      ColumnLayout {
        id: iconTextLayout

        required property int index
        required property var modelData

        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.maximumHeight: 84
        Layout.topMargin: 4
        spacing: 0

        Componenets.IconButton {
          id: icon

          Layout.fillWidth: true
          Layout.preferredHeight: 48
          Layout.preferredWidth: 1
          iconPath: iconTextLayout.modelData.icon
          isActive: navBar.currentIndex === iconTextLayout.index
          navPath: iconTextLayout.modelData.target
          pageArgs: {}

          background: Rectangle {
            anchors.centerIn: icon
            color: icon.isActive ? "#9F9FFF" : "transparent"
            height: icon.height + 4
            opacity: 0.1
            radius: height / 2
            width: icon.width - 16
          }

          onClicked: {
            navBar.currentIndex = iconTextLayout.index;
          }
        }
        Text {
          Layout.fillWidth: true
          Layout.preferredHeight: 16
          Layout.preferredWidth: 1
          color: navBar.currentIndex === iconTextLayout.index ? "#9F9FFF" : "#FFFFFF"
          font.pixelSize: 10
          horizontalAlignment: Text.AlignHCenter
          text: iconTextLayout.modelData.text
          verticalAlignment: Text.AlignTop
        }
      }
    }
    Item {
      id: lowerSpacer

      Layout.fillHeight: true
    }
  }
}
