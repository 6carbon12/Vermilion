#include "PlaylistsModel.h"

namespace Models {

PlaylistsModel::PlaylistsModel(QObject *parent) : QAbstractListModel(parent) {}

int PlaylistsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return static_cast<int>(m_playlists.size());
}

QVariant PlaylistsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= m_playlists.size()) {
    return QVariant();
  }

  const Core::Playlist &playlist = m_playlists.at(index.row());

  switch (role) {
  case PlaylistRole:
    return QVariant::fromValue(playlist);
  case IdRole:
    return playlist.id;
  case NameRole:
    return playlist.name;
  case ArtUrlRole:
    return playlist.artUrl;
  case TypeRole:
    return static_cast<int>(playlist.type);
  case CreatedAtRole:
    return playlist.createdAt;
  case Qt::DisplayRole:
    return playlist.name;
  default:
    return QVariant();
  }
}

QHash<int, QByteArray> PlaylistsModel::roleNames() const {
  return {{PlaylistRole, "playlist"},
          {IdRole, "id"},
          {NameRole, "name"},
          {ArtUrlRole, "artUrl"},
          {TypeRole, "type"},
          {CreatedAtRole, "createdAt"}};
}

void PlaylistsModel::setPlaylists(const QList<Core::Playlist> &playlists) {
  beginResetModel();
  m_playlists = playlists;
  endResetModel();
}
} // namespace Models
