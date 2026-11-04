#pragma once

#include "Playlist.h"
#include "PlaylistEngine.h"
#include "PlaylistsModel.h"
#include "TrackListModel.h"
#include "Track.h"
#include <QList>
#include <QObject>
#include <QQmlEngine>

class PlaylistsController : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON
  QML_NAMED_ELEMENT(PlaylistsController)

  Q_PROPERTY(Models::PlaylistsModel *model READ getModel NOTIFY playlistUpdated)
  Q_PROPERTY(QString lastError READ lastError NOTIFY errorOccurred)

public:
  ~PlaylistsController() override = default;
  static PlaylistsController *instance();
  static PlaylistsController *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

  Models::PlaylistsModel *getModel();
  QString lastError() const;

  Q_INVOKABLE bool addToPlaylist(const Core::Track &track, const QString &playlistId);
  Q_INVOKABLE bool addToPlaylist(const QList<Core::Track> &track, const QString &playlistId);
  Q_INVOKABLE bool createPlaylist(const QString &playlistName);
  Q_INVOKABLE bool deletePlaylist(const QString &playlistId);
  Q_INVOKABLE bool removeFromPlaylist(const QString &trackId, const QString &playlistId);
  Q_INVOKABLE bool renamePlaylist(const QString &playlistId, const QString &newName);
  Q_INVOKABLE bool updatePlaylistArt(const QString &playlistId, const QString &artPath);

  Q_INVOKABLE QList<Core::Playlist> getPlaylists() const;
  Q_INVOKABLE Core::Playlist getPlaylistFromId(const QString &playlistId) const;
  Q_INVOKABLE Models::TrackListModel* getTracksFromPlaylist(const QString &playlistId) const;
  Q_INVOKABLE QList<Core::Track> getTracksListFromPlaylist(const QString &playlistId) const;
  Q_INVOKABLE QString getLastError() const;

Q_SIGNALS:
  void errorOccurred();
  void tracksUpdated(const QString &playlistId);
  void playlistUpdated(const QString &playlistId);

private:
  explicit PlaylistsController(QObject *parent = nullptr);
  PlaylistEngine m_engine;
  Models::PlaylistsModel m_model;
};
