pragma Singleton
import QtQuick
import "../components"

QtObject {
  property StandardSizes fontSize: StandardSizes {
    small: 10
    base: 16
    large: 24
  }
  property StandardSizes iconSize: StandardSizes {
    small: 24
    base: 32
    large: 96
  }
}
