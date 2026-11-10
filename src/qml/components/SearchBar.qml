import QtQuick
import QtQuick.Controls
import Vermilion

SearchField {
  id: searchBar

  property alias query: textInput.text

  searchIndicator.indicator: null

  background: Rectangle {
    anchors.fill: parent
    color: palette.alternateBase
    radius: parent.height / 2
  }
  clearIndicator.indicator: IconButton {
    anchors.right: searchBar.right
    anchors.rightMargin: 12
    anchors.verticalCenter: parent.verticalCenter
    height: Design.iconSize.small
    width: Design.iconSize.small
    size: Qt.size(height, width)
    hasState: false
    source: "qrc:/icons/cross.svg"
    visible: searchBar.text !== ""

    background: Rectangle {
      color: palette.alternateBase
      radius: parent.height / 2
    }

    onClicked: {
      searchBar.query = "";
    }
  }
  contentItem: TextField {
    id: textInput

    EnterKey.type: Qt.EnterKeySearch
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
