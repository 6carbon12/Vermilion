import QtQuick
import QtQuick.Controls

SearchField {
  id: searchBar

  searchIndicator.indicator: null
  property alias query: textInput.text

  clearIndicator.indicator: IconButton {
    anchors.right: searchBar.right
    anchors.rightMargin: 8
    anchors.verticalCenter: parent.verticalCenter
    hasState: false
    height: 42
    size: Qt.size(32, 32)
    source: "../../../assets/icons/cross.svg"
    visible: searchBar.text !== ""

    background: Rectangle {
      color: palette.alternateBase
      radius: parent.height / 2
    }
  }
  background: Rectangle {
    anchors.fill: parent
    color: palette.alternateBase
    radius: parent.height / 2
  }
  contentItem: TextField {
    id: textInput
    color: palette.text
    font.pixelSize: 14
    placeholderText: "Type to search"
    placeholderTextColor: Qt.darker(palette.text, 2)

    background: Rectangle {
      anchors.fill: parent
      color: palette.alternateBase
      radius: parent.height / 2
    }
  }
}
