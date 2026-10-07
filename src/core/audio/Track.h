#pragma once

#include <QMetaType>
#include <QString>

namespace Core {

struct Track {
  QString url;
  QString title;
  QString views;
  QString artist;
  QString thumbnailUrl;
  QString duration;

  // The compiler automatically generates the default constructor,
  // copy constructor, and assignment operator for you. You don't need to write them!

  // Keep your custom equality operators since you only want to compare by URL
  bool operator==(const Track &other) const { return url == other.url; }
  bool operator!=(const Track &other) const { return !(*this == other); }
};

} // namespace Core

// Keep this so you can pass Core::Track through Qt Signals and Slots in C++
Q_DECLARE_METATYPE(Core::Track)
