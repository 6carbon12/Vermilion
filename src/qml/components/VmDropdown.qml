pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts
import Vermilion

Rectangle {
  id: root

  property Component caret: defaultCaret
  property Component delegate: defaultDelegate
  property alias isOpen: dropDownView.visible
  property alias itemHeight: dropDownView.itemHeight
  property int itemInnerMargins: 16
  property int itemWidth: {
    if (!root.options || root.options.length === 0)
      return 0;
    const longestItem = root.options.reduce((a, b) => a.length > b.length ? a : b);
    return baseFontMetrics.advanceWidth(longestItem) + (itemInnerMargins * 2) + 64;
  }
  property var options: []
  property string selected: options && options.length > 0 ? options[0] : ""

  // Custom signal to emit when an item is selected
  signal itemSelected(string itemData)

  function close() {
    dropDownView.visible = false;
  }
  function open() {
    dropDownView.visible = true;
  }

  color: "transparent"
  radius: 8

  MouseArea {
    id: outsideDismissArea

    anchors.fill: parent
    enabled: dropDownView.visible
    parent: root.Overlay.overlay
    z: 0

    onClicked: {
      console.log("Closing window");
      root.close();
    }
  }
  FontMetrics {
    id: baseFontMetrics

    font.pixelSize: Design.fontSize.base
    font.weight: Font.DemiBold
  }
  MouseArea {
    anchors.fill: sortOptionDisplay

    onClicked: {
      dropDownView.visible = !dropDownView.visible;
    }
  }
  RowLayout {
    id: sortOptionDisplay

    anchors.fill: parent
    spacing: 12

    Text {
      Layout.fillHeight: true
      color: palette.text
      font.pixelSize: Design.fontSize.base
      font.weight: Font.DemiBold
      text: root.selected
      verticalAlignment: Text.AlignVCenter
    }
    Loader {
      Layout.preferredHeight: Design.fontSize.base
      Layout.preferredWidth: height
      sourceComponent: root.caret
    }
    Item {
      id: rightSpacer

      Layout.fillWidth: true
    }
  }
  Rectangle {
    id: dropDownView

    readonly property int itemHeight: Design.fontSize.base + (2 * margins) + 8
    readonly property int margins: 16

    anchors.top: sortOptionDisplay.bottom
    color: palette.alternateBase
    height: root.options ? (root.options.length * (itemHeight)) : 0
    // Need to reparent so this view can go on top of the MouseArea.
    parent: root.Overlay.overlay
    radius: root.radius
    visible: false
    width: {
      if (!root.options || root.options.length === 0)
        return 0;
      const longestItem = root.options.reduce((a, b) => a.length > b.length ? a : b);
      return baseFontMetrics.advanceWidth(longestItem) + (margins * 2) + 64;
    }
    z: 1

    onVisibleChanged: {
      if (visible && root.Overlay.overlay) {
        const posBelow = root.mapToItem(root.Overlay.overlay, 0, sortOptionDisplay.height);
        const posAbove = root.mapToItem(root.Overlay.overlay, 0, 0);

        const overlayHeight = root.Overlay.overlay.height;
        const spaceBelow = overlayHeight - posBelow.y;

        if (spaceBelow >= dropDownView.height || posBelow.y + dropDownView.height <= overlayHeight) {
          dropDownView.x = posBelow.x;
          dropDownView.y = posBelow.y;
        } else {
          dropDownView.x = posAbove.x;
          dropDownView.y = posAbove.y - dropDownView.height;
        }
      }
    }

    Column {
      id: optionsCol

      anchors.bottomMargin: spacing
      anchors.fill: parent
      anchors.topMargin: spacing

      Repeater {
        delegate: root.delegate
        model: root.options
      }
    }
  }
  Component {
    id: defaultDelegate

    Rectangle {
      id: repeaterItem

      required property int index

      // Expose values derived from root or dropDownView context
      property bool isSelected: repeaterItem.modelData === root.selected
      required property string modelData

      color: repeaterItem.isSelected ? Qt.lighter(palette.alternateBase, 2) : "transparent"
      height: dropDownView.itemHeight
      width: dropDownView.width

      RowLayout {
        anchors.fill: parent
        anchors.margins: dropDownView.margins

        Text {
          Layout.preferredHeight: parent.height
          color: palette.text
          font.pixelSize: Design.fontSize.base
          font.weight: Font.DemiBold
          text: repeaterItem.modelData
          verticalAlignment: Text.AlignVCenter
        }
      }
      MouseArea {
        anchors.fill: parent

        onClicked: {
          root.itemSelected(repeaterItem.modelData);
          root.selected = repeaterItem.modelData;
        }
      }
    }
  }
  Component {
    id: defaultCaret

    Image {
      layer.enabled: true
      source: Qt.resolvedUrl("../../../assets/icons/caret-down-fill.svg")
      sourceSize: Qt.size(height, width)

      layer.effect: MultiEffect {
        brightness: 1.0
        colorization: 1.0
        colorizationColor: palette.text
      }
    }
  }
}
