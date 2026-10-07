#include "TrackListModel.h"
#include "PlayerController.h"
#include <qloggingcategory.h>

namespace Models {
Q_LOGGING_CATEGORY(trackListModel, "TrackListModel");

TrackListModel::TrackListModel(QObject *parent) : QAbstractListModel(parent) {}

TrackListModel::TrackListModel(const QList<Core::Track> &tracks, QObject *parent) : QAbstractListModel(parent) {
  m_tracks = tracks;
}

void TrackListModel::setTracks(const QList<Core::Track> &tracks) {
  beginResetModel();
  m_tracks = tracks;
  endResetModel();
}

int TrackListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }
  return m_tracks.count();
}

QHash<int, QByteArray> TrackListModel::roleNames() const {
  QHash<int, QByteArray> roles;

  roles[UrlRole] = "url";
  roles[TitleRole] = "title";
  roles[ViewsRole] = "views";
  roles[ArtistRole] = "artist";
  roles[ThumbnailUrlRole] = "thumbnailUrl";
  roles[DurationRole] = "duration";

  return roles;
}

QVariant TrackListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() > m_tracks.count() - 1) {
    return QVariant();
  }

  const Core::Track &track = m_tracks[index.row()];
  switch (role) {
  case UrlRole:
    return track.url;
  case TitleRole:
    return track.title;
  case ViewsRole:
    return track.views;
  case ArtistRole:
    return track.artist;
  case ThumbnailUrlRole:
    return track.thumbnailUrl;
  case DurationRole:
    return track.duration;
  }

  qCWarning(trackListModel) << "Requesting unknown role. Returning default QVariant";
  return QVariant();
}

void TrackListModel::playTrackAtIndex(const int index) {
  if (index < 0 || index >= m_tracks.count()) {
    qCWarning(trackListModel) << "Play requested for invalid index:" << index << ". Not doing anything";
    return;
  }
  PlayerController::instance()->setUrl(m_tracks[index].url);
  PlayerController::instance()->play();
}
} // namespace Models
