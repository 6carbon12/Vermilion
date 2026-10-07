#pragma once

#include "Track.h"
#include <QAbstractListModel>
#include <qlist.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace Models {

class TrackListModel : public QAbstractListModel {
  Q_OBJECT
  QML_ELEMENT

public:
  explicit TrackListModel(QObject *parent = nullptr);
  explicit TrackListModel(const QList<Core::Track> &tracks, QObject *parent = nullptr);

  enum TrackRoles {
    UrlRole = Qt::UserRole + 1,
    TitleRole,
    ViewsRole,
    ArtistRole,
    ThumbnailUrlRole,
    DurationRole,
  };

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void playTrackAtIndex(const int index);

private:
  QList<Core::Track> m_tracks;
};
} // namespace Models
