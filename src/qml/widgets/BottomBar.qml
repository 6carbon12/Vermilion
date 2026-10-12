pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import "../components" as Componenets

RowLayout {
  id: navBar

  property int currentIndex: 0

  anchors.fill: parent
  spacing: 0

  Repeater {
    model: [
      {
        icon: "../../../assets/icons/home",
        target: Qt.resolvedUrl("../pages/Home.qml")
      },
      {
        icon: "../../../assets/icons/search",
        target: Qt.resolvedUrl("../pages/Search.qml")
      },
      {
        icon: "../../../assets/icons/library",
        target: Qt.resolvedUrl("../pages/Library.qml")
      },
      {
        icon: "../../../assets/icons/setting",
        target: Qt.resolvedUrl("../pages/Settings.qml")
      }
    ]

    Componenets.IconButton {
      required property int index
      required property var modelData

      Layout.fillWidth: true
      Layout.preferredHeight: 48
      Layout.preferredWidth: 1
      iconPath: modelData.icon
      isActive: navBar.currentIndex === index
      navPath: modelData.target
      pageArgs: {}

      onClicked: {
        navBar.currentIndex = index;
      }
    }
  }
}
