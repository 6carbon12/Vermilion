#pragma once

#include <QMetaType>
#include <QString>
#include "Artist.h"

namespace Core {

struct Track {
  QString id;
  bool isExplict;
  QString url;
  QString title;
  QString views;
  QString coverArtUrl;
  QString durationString;
  long duration;
  // 8 is just an arbitrary large number.
  Artist artists[8];
  int artistsNum;
};

} // namespace Core
