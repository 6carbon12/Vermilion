#pragma once

#include "Artist.h"
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace Core {

class Track {
  Q_GADGET
  QML_ELEMENT

  Q_PROPERTY(QString id MEMBER id)
  Q_PROPERTY(bool isExplicit MEMBER isExplict)
  Q_PROPERTY(QString url MEMBER url)
  Q_PROPERTY(QString title MEMBER title)
  Q_PROPERTY(QString views MEMBER views)
  Q_PROPERTY(QString coverArtUrl MEMBER coverArtUrl)
  Q_PROPERTY(QString durationString MEMBER durationString)
  Q_PROPERTY(long duration MEMBER duration)
  Q_PROPERTY(QList<Core::Artist> artists READ artistsAsList)

public:
  QString id;
  bool isExplict{false};
  QString url;
  QString title;
  QString views;
  QString coverArtUrl;
  QString durationString;
  long duration{0};

  Artist artists[8];
  int artistsNum{0};

  QList<Core::Artist> artistsAsList() const {
    QList<Core::Artist> list;
    list.reserve(artistsNum);
    for (int i = 0; i < artistsNum && i < 8; ++i) {
      list.append(artists[i]);
    }
    return list;
  }

  bool operator==(const Track& other) const {
    return id == other.id;
  }
};

} // namespace Core

Q_DECLARE_METATYPE(Core::Track)
