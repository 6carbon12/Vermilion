import QtQuick
import QtQuick.Layouts
import "../components"

RowLayout {
  id: root

  Layout.fillWidth: true
  Layout.preferredHeight: 32

  signal leftClicked();
  signal rightClicked();

  property alias leftIcon: leftButton.source
  property alias rightIcon: rightButton.source

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
