#include "TrackListModel.h"
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

QList<Core::Track> TrackListModel::getTracks() const {
  return m_tracks;
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
  roles[TrackRole] = "track";
  roles[ViewsRole] = "views";
  roles[ArtistsRole] = "artists";
  roles[ArtistsNumRole] = "artistsSize";
  roles[ThumbnailUrlRole] = "thumbnailUrl";
  roles[DurationRole] = "duration";

  return roles;
}

QVariant TrackListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() > m_tracks.count() - 1) {
    return QVariant();
  }

  const Core::Track &track = m_tracks[index.row()];
  QList<Core::Artist> artistList;
  switch (role) {
  case TrackRole:
    return QVariant::fromValue(track);
  case UrlRole:
    return track.url;
  case TitleRole:
    return track.title;
  case ViewsRole:
    return track.views;
  case ArtistsRole:
    for (int i = 0; i < track.artistsNum; i++) {
      artistList.append(track.artists[i]);
    }
    return QVariant::fromValue(artistList);
  case ArtistsNumRole:
    return track.artistsNum;
  case ThumbnailUrlRole:
    return track.coverArtUrl;
  case DurationRole:
    return track.durationString;
  }

  qCWarning(trackListModel) << "Requesting unknown role. Returning default QVariant";
  return QVariant();
}
} // namespace Models
