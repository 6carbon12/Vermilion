#pragma once

#include "Playlist.h"
#include <QAbstractListModel>
#include <QList>

class PlaylistEngine;

namespace Models {
class PlaylistsModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum PlaylistRoles { PlaylistRole = Qt::UserRole + 1, IdRole, NameRole, ArtUrlRole, TypeRole, CreatedAtRole };
  Q_ENUM(PlaylistRoles)

  explicit PlaylistsModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;
  void setPlaylists(const QList<Core::Playlist> &playlists);

private:
  QList<Core::Playlist> m_playlists;
};
} // namespace Models
