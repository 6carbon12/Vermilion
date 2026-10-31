#pragma once

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <qqmlintegration.h>

namespace Core {

enum class PlaylistType { User, Liked };

inline QString playlistTypeToString(PlaylistType type) {
  switch (type) {
  case PlaylistType::Liked:
    return QStringLiteral("liked");
  case PlaylistType::User:
  default:
    return QStringLiteral("user");
  }
}

inline PlaylistType playlistTypeFromString(const QString &str) {
  if (str == QStringLiteral("liked")) {
    return PlaylistType::Liked;
  }
  return PlaylistType::User;
}

struct Playlist {
  Q_GADGET
  QML_ELEMENT

  Q_PROPERTY(QString id MEMBER id)
  Q_PROPERTY(QString name MEMBER name)
  Q_PROPERTY(QString artUrl MEMBER artUrl)
  Q_PROPERTY(Core::PlaylistType type MEMBER type)
  Q_PROPERTY(QString typeString READ typeString)
  Q_PROPERTY(QDateTime createdAt MEMBER createdAt)

  Q_ENUM(PlaylistType);
public:
  QString id;
  QString name;
  QString artUrl;
  PlaylistType type{PlaylistType::User};
  QDateTime createdAt;

  QString typeString() const { return playlistTypeToString(type); }
};

} // namespace Core
