#pragma once

#include <QString>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace Core {
class Track : public QObject {
  Q_GADGET
  QML_VALUE_TYPE(track)
  Q_PROPERTY(QString url MEMBER url)
  Q_PROPERTY(QString title MEMBER title)
  Q_PROPERTY(QString views MEMBER views)
  Q_PROPERTY(QString artist MEMBER artist)
  Q_PROPERTY(QString thumbnailUrl MEMBER thumbnailUrl)
  Q_PROPERTY(QString duration MEMBER duration)
public:
  QString url{};
  QString title{};
  QString views{};
  QString artist{};
  QString thumbnailUrl{};
  QString duration{};

  Track() {};
  Track(const Track &other) :
    url(other.url),
    title(other.title),
    views(other.views),
    artist(other.artist),
    thumbnailUrl(other.thumbnailUrl),
    duration(other.duration) {}

  Track &operator=(const Track &other) {
    this->url = other.url;
    this->title = other.title;
    this->views = other.views;
    this->artist = other.artist;
    this->thumbnailUrl = other.thumbnailUrl;
    this->duration = other.duration;
    return *this;
  }

  bool operator==(const Track &other) const { return url == other.url; }
  bool operator!=(const Track &other) const { return !(*this == other); }
};
} // namespace Core

Q_DECLARE_METATYPE(Core::Track)
