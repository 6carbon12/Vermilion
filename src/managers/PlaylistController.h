#pragma once

#include "Playlist.h"
#include "PlaylistEngine.h"
#include "PlaylistsModel.h"
#include "Track.h"
#include <QList>
#include <QObject>
#include <QQmlEngine>

class PlaylistsController : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON
  QML_NAMED_ELEMENT(PlaylistsController)

  Q_PROPERTY(Models::PlaylistsModel *model READ getModel NOTIFY modelChanged)
  Q_PROPERTY(QString lastError READ lastError NOTIFY errorOccurred)

public:
  explicit PlaylistsController(QObject *parent = nullptr);

  Models::PlaylistsModel *getModel();
  QString lastError() const;

  Q_INVOKABLE bool addToPlaylist(const Core::Track &track, const QString &playlistId);
  Q_INVOKABLE bool createPlaylist(const QString &playlistName);
  Q_INVOKABLE bool deletePlaylist(const QString &playlistId);
  Q_INVOKABLE bool removeFromPlaylist(const QString &trackId, const QString &playlistId);
  Q_INVOKABLE bool renamePlaylist(const QString &playlistId, const QString &newName);
  Q_INVOKABLE bool updatePlaylistArt(const QString &playlistId, const QString &artPath);

  Q_INVOKABLE QList<Core::Playlist> getPlaylists() const;
  Q_INVOKABLE QList<Core::Track> getTracksFromPlaylist(const QString &playlistId) const;
  Q_INVOKABLE QString getLastError() const;

Q_SIGNALS:
  void modelChanged();
  void errorOccurred();
  void tracksUpdated(const QString &playlistId);
  void playlistUpdated(const QString &playlistId);

private:
  PlaylistEngine m_engine;
  Models::PlaylistsModel m_model;
};
