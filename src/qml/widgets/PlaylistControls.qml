import QtQuick
import QtQuick.Layouts
import Vermilion
import "../components"

RowLayout {
  id: root

  Layout.fillWidth: true
  Layout.maximumHeight: 128
  Layout.preferredHeight: 128
  Layout.topMargin: -8
  spacing: 12

  signal download();
  signal play();
  signal edit();

  Item {
    id: controlsLeftSpacer

    Layout.fillWidth: true
  }
  CircleIconButton {
    id: downloadButton

    Layout.alignment: Qt.AlignHCenter
    Layout.bottomMargin: 32
    Layout.fillHeight: true
    Layout.preferredWidth: height
    Layout.topMargin: 32
    iconSource: "qrc:/icons/download.svg"

    onClicked: {
      root.download()
    }
  }
  IconButton {
    id: playButton

    Layout.fillHeight: true
    Layout.preferredWidth: height
    hasState: false
    size: Qt.size(Design.iconSize.large, Design.iconSize.large)
    source: "qrc:/icons/play-circle.svg"

    onClicked: {
      root.play()
    }
  }
  CircleIconButton {
    id: editButton

    Layout.alignment: Qt.AlignHCenter
    Layout.bottomMargin: 32
    Layout.fillHeight: true
    Layout.preferredWidth: height
    Layout.topMargin: 32
    iconSource: "qrc:/icons/pen.svg"

    onClicked: {
      root.edit()
    }
  }
  Item {
    id: controlsRightSpacer

    Layout.fillWidth: true
  }
}
