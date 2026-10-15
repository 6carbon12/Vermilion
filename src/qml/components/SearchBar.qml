import QtQuick
import QtQuick.Controls

SearchField {
  id: searchBar

  placeholderText: "Type to search"
  searchIndicator.indicator: null

  background: Rectangle {
    anchors.fill: parent
    color: palette.alternateBase
    radius: parent.height / 2
  }
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
}
