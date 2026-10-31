#include "PlaylistController.h"

PlaylistsController::PlaylistsController(QObject *parent) : QObject(parent) {
  connect(&m_engine, &PlaylistEngine::tracksUpdated, this, &PlaylistsController::tracksUpdated);
  connect(&m_engine, &PlaylistEngine::playlistUpdated, this, &PlaylistsController::modelChanged);
}

Models::PlaylistsModel *PlaylistsController::getModel() {
  QList<Core::Playlist> playlists = getPlaylists();
  m_model.setPlaylists(playlists);
  return &m_model;
}

QString PlaylistsController::lastError() const { return m_engine.getLastError(); }

bool PlaylistsController::addToPlaylist(const Core::Track &track, const QString &playlistId) {
  bool ok = m_engine.addToPlaylist(track, playlistId);
  if (!ok)
    Q_EMIT errorOccurred();
  return ok;
}

bool PlaylistsController::createPlaylist(const QString &playlistName) {
  Core::Playlist pt;
  pt.name = playlistName;
  pt.artUrl = "qrc:/icons/defaultPlaylist.svg";
  pt.type = Core::PlaylistType::User;
  pt.id = m_engine.generateLocalPlaylistId();
  bool ok = m_engine.createPlaylist(pt);
  if (!ok)
    Q_EMIT errorOccurred();
  return ok;
}

bool PlaylistsController::deletePlaylist(const QString &playlistId) {
  bool ok = m_engine.deletePlaylist(playlistId);
  if (!ok)
    Q_EMIT errorOccurred();
  return ok;
}

bool PlaylistsController::removeFromPlaylist(const QString &trackId, const QString &playlistId) {
  bool ok = m_engine.removeFromPlaylist(trackId, playlistId);
  if (!ok)
    Q_EMIT errorOccurred();
  return ok;
}

bool PlaylistsController::renamePlaylist(const QString &playlistId, const QString &newName) {
  bool ok = m_engine.renamePlaylist(playlistId, newName);
  if (!ok)
    Q_EMIT errorOccurred();
  return ok;
}

bool PlaylistsController::updatePlaylistArt(const QString &playlistId, const QString &artPath) {
  bool ok = m_engine.updatePlaylistArt(playlistId, artPath);
  if (!ok)
    Q_EMIT errorOccurred();
  return ok;
}

QList<Core::Playlist> PlaylistsController::getPlaylists() const { return m_engine.getPlaylists(); }

QList<Core::Track> PlaylistsController::getTracksFromPlaylist(const QString &playlistId) const {
  return m_engine.getTracksFromPlaylist(playlistId);
}

QString PlaylistsController::getLastError() const { return m_engine.getLastError(); }
