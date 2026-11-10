pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts

Item {
  id: root

  required property real globalProgress
  readonly property real shimmerBeamX: (globalProgress * 300) - 100

  Layout.fillWidth: true
  height: 64

  RowLayout {
    anchors.fill: parent
    spacing: 16

    Rectangle {
      id: albumCoverSkeleton

      Layout.preferredHeight: 64
      Layout.preferredWidth: 64
      clip: true 

      color: palette.alternateBase
      radius: 4

      Rectangle {
        color: "transparent"
        height: parent.height
        width: 80
        x: root.shimmerBeamX - parent.x
        y: 0

        gradient: Gradient {
          orientation: Gradient.Horizontal

          GradientStop {
            color: "transparent"
            position: 0.0
          }
          GradientStop {
            color: Qt.lighter(root.palette.alternateBase, 1.6)
            position: 0.5
          }
          GradientStop {
            color: "transparent"
            position: 1.0
          }
        }
      }
    }

    Column {
      Layout.alignment: Qt.AlignVCenter
      Layout.fillWidth: true
      spacing: 6

      Rectangle {
        clip: true
        color: palette.alternateBase
        height: 14
        radius: 3
        width: 160

        Rectangle {
          color: "transparent"
          height: parent.height
          width: 80
          x: root.shimmerBeamX - parent.x
          y: 0

          gradient: Gradient {
            orientation: Gradient.Horizontal

            GradientStop {
              color: "transparent"
              position: 0.0
            }
            GradientStop {
              color: Qt.lighter(root.palette.alternateBase, 1.6)
              position: 0.5
            }
            GradientStop {
              color: "transparent"
              position: 1.0
            }
          }
        }
      }

      Rectangle {
        clip: true
        color: palette.alternateBase
        height: 12
        radius: 3
        width: 100

        Rectangle {
          color: "transparent"
          height: parent.height
          width: 80
          x: root.shimmerBeamX - parent.x
          y: 0

          gradient: Gradient {
            orientation: Gradient.Horizontal

            GradientStop {
              color: "transparent"
              position: 0.0
            }
            GradientStop {
              color: Qt.lighter(root.palette.alternateBase, 1.6)
              position: 0.5
            }
            GradientStop {
              color: "transparent"
              position: 1.0
            }
          }
        }
      }
    }
  }
}
