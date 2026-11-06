#pragma once

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <qqmlintegration.h>

namespace Core {
struct Playlist {
  Q_GADGET
  QML_ELEMENT

  Q_PROPERTY(QString id MEMBER id)
  Q_PROPERTY(QString name MEMBER name)
  Q_PROPERTY(QString artUrl MEMBER artUrl)
  Q_PROPERTY(Core::PlaylistType type MEMBER type)
  Q_PROPERTY(QDateTime createdAt MEMBER createdAt)
  Q_PROPERTY(TrackSortCriterion sortBy MEMBER sortBy)
  Q_PROPERTY(bool isReverse MEMBER isReverse)

public:
  /// @brief Type of playlist, represents the operations possible on a given playlist by the user.
  ///
  /// Each playlist has the following operations possible.
  /// All possible operations on a type are represented as a commoa separated list in it's documentation.
  ///
  /// - `create`: Weather this `type` of playlist can be created by user, is only available for `User` type.
  /// - `delete`: Weather this `type` can be deleted, is only available for `User` type.
  /// - `rename`: Weather this `type` can be renamed by user.
  /// - `view`: Weather this `type` is visible in user's library.
  ///           Used for implementation-only playlists.
  /// - `add`: Weather songs can be added to this `type`.
  /// - `remove`: Weather songs can be removed from this `type`.
  enum class PlaylistType {
    User,   ///< create, delete, rename, view, add, remove.
    Liked,  ///< rename, view, add, remove.
    Current ///< add, remove.
  };

  /// @brief All sort methods inherently assume ascending order.
  enum class TrackSortCriterion {
    Title,
    Artist,
    Duration,
    DateAdded,
    Custom,
    Random,
  };

  Q_ENUM(PlaylistType);
  Q_ENUM(TrackSortCriterion);

  QString id;
  QString name;
  QString artUrl;
  PlaylistType type{PlaylistType::User};
  QDateTime createdAt;
  TrackSortCriterion sortBy;
  bool isReverse;

  bool operator==(const Playlist &other) const {
    return id == other.id && type == other.type && name == other.name && artUrl == other.artUrl &&
           createdAt == other.createdAt && sortBy == other.sortBy && isReverse == other.isReverse;
  }
};

using PlaylistType = Playlist::PlaylistType;
using TrackSortCriterion = Playlist::TrackSortCriterion;

inline QString playlistTypeToString(PlaylistType type) {
  switch (type) {
  case PlaylistType::Liked:
    return QStringLiteral("liked");
  case PlaylistType::Current:
    return QStringLiteral("current");
  case PlaylistType::User:
  default:
    return QStringLiteral("user");
  }
}

inline PlaylistType playlistTypeFromString(const QString &str) {
  const QString lowerStr = str.toLower();
  if (lowerStr == QStringLiteral("liked")) {
    return PlaylistType::Liked;
  } else if (lowerStr == QStringLiteral("current")) {
    return PlaylistType::Current;
  }
  return PlaylistType::User;
}

inline QString sortCriterionToString(TrackSortCriterion type) {
  switch (type) {
  case TrackSortCriterion::Title:
    return QStringLiteral("title");
  case TrackSortCriterion::Artist:
    return QStringLiteral("artist");
  case TrackSortCriterion::Duration:
    return QStringLiteral("duration");
  case TrackSortCriterion::DateAdded:
    return QStringLiteral("dateadded");
  case TrackSortCriterion::Custom:
    return QStringLiteral("custom");
  case TrackSortCriterion::Random:
    return QStringLiteral("random");
  }
  return QStringLiteral("dateadded");
}

inline TrackSortCriterion stringToSortCriterion(const QString &str) {
  const QString lowerStr = str.toLower();

  if (lowerStr == QStringLiteral("title"))
    return TrackSortCriterion::Title;
  if (lowerStr == QStringLiteral("artist"))
    return TrackSortCriterion::Artist;
  if (lowerStr == QStringLiteral("duration"))
    return TrackSortCriterion::Duration;
  if (lowerStr == QStringLiteral("dateadded"))
    return TrackSortCriterion::DateAdded;
  if (lowerStr == QStringLiteral("random"))
    return TrackSortCriterion::Random;
  if (lowerStr == QStringLiteral("custom"))
    return TrackSortCriterion::Custom;

  return TrackSortCriterion::DateAdded;
}

} // namespace Core

Q_DECLARE_METATYPE(Core::Playlist)
