#pragma once

#include <QMetaType>
#include <QString>

namespace Core {

struct Track {
  QString id;
  bool isExplict;
  QString url;
  QString title;
  QString views;
  QString artist;
  QString coverArtUrl;
  QString durationString;
  long duration;

  bool operator==(const Track &other) const { return url == other.url; }
  bool operator!=(const Track &other) const { return !(*this == other); }
};

} // namespace Core
