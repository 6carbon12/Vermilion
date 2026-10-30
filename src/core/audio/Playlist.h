#pragma once

#include <QDateTime>
#include <QMetaType>
#include <QString>

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
  QString id;
  QString name;
  QString artUrl;
  PlaylistType type;
  QDateTime createdAt;
};

} // namespace Core
