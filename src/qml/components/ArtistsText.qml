pragma ComponentBehavior: Bound

import QtQuick
import Vermilion

Text {
  id: root

  property list<Artist> artists
  property int artistsSize: root.artists ? root.artists.length : 0
  property int fontSize: 14
  property color textColor: palette.text
  property bool wrapable: false

  color: root.textColor
  elide: wrapable ? Text.ElideNone : Text.ElideRight
  font.pixelSize: root.fontSize
  maximumLineCount: wrapable ? 2 : 1
  text: {
    if (!root.artists || root.artistsSize === 0)
      return "";
    let result = "";

    for (let i = 0; i < root.artistsSize; ++i) {
      let artist = root.artists[i];
      if (!artist)
        continue;

      result += (artist.name || "");

      if (i < root.artistsSize - 2) {
        result += ", ";
      } else if (i === root.artistsSize - 2) {
        result += " & ";
      }
    }
    return result;
  }
  textFormat: Text.PlainText
  wrapMode: wrapable ? Text.WordWrap : Text.NoWrap
}
