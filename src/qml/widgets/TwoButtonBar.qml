import QtQuick
import QtQuick.Layouts
import "../components"

Rectangle {
  id: root

  property alias leftIcon: leftButton.source

  // Exposed middle Text item properties
  property alias middleText: middleTitle.text
  property alias middleTextColor: middleTitle.color
  property alias middleTextOpacity: middleTitle.opacity
  property alias rightIcon: rightButton.source

  signal leftClicked
  signal rightClicked

  Layout.fillWidth: true
  Layout.preferredHeight: 32
  color: "transparent"

  RowLayout {
    anchors.fill: parent

    IconButton {
      id: leftButton

      Layout.preferredHeight: parent.height
      Layout.preferredWidth: parent.height
      hasState: false

      onClicked: {
        root.leftClicked();
      }
    }
    Item {
      id: topBarMiddleSpacer

      Layout.fillWidth: true

      Text {
        id: middleTitle

        anchors.centerIn: parent
        elide: Text.ElideRight
        font.bold: true
        font.pixelSize: 16
        horizontalAlignment: Text.AlignHCenter
        width: parent.width
      }
    }
    IconButton {
      id: rightButton

      Layout.preferredHeight: parent.height
      Layout.preferredWidth: parent.height
      hasState: false

      onClicked: {
        root.rightClicked();
      }
    }
  }
}
