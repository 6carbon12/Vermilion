#include "PlaylistController.h"
#include "TrackListModel.h"
#include <QLoggingCategory>

using Core::Playlist;

Q_LOGGING_CATEGORY(PlaylistController_l, "PlaylistController")

PlaylistsController::PlaylistsController(QObject *parent) : QObject(parent) {
  connect(&m_engine, &PlaylistEngine::tracksUpdated, this, &PlaylistsController::tracksUpdated);
  connect(&m_engine, &PlaylistEngine::playlistUpdated, this, &PlaylistsController::playlistUpdated);
}

PlaylistsController *PlaylistsController::instance() {
  static PlaylistsController _instance;
  return &_instance;
}

PlaylistsController *PlaylistsController::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
  Q_UNUSED(qmlEngine);
  Q_UNUSED(jsEngine);
  PlaylistsController *inst = PlaylistsController::instance();
  QQmlEngine::setObjectOwnership(inst, QQmlEngine::CppOwnership);
  return inst;
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

bool PlaylistsController::addToPlaylist(const QList<Core::Track> &track, const QString &playlistId) {
  bool ok = m_engine.addToPlaylist(track, playlistId);
  if (!ok)
    Q_EMIT errorOccurred();
  return ok;
}

bool PlaylistsController::createPlaylist(const QString &playlistName) {
  Core::Playlist pt;
  pt.name = playlistName;
  pt.artUrl = "qrc:images/default-playlist-cover.png";
  pt.type = Core::PlaylistType::User;
  pt.id = m_engine.generateLocalPlaylistId();
  bool ok = m_engine.createPlaylist(pt);
  if (!ok)
    Q_EMIT errorOccurred();
  return ok;
}

bool PlaylistsController::createPlaylist(const Core::Playlist &playlist) {
  bool ok = m_engine.createPlaylist(playlist);
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

bool PlaylistsController::isLiked(const QString trackId) {
  return m_engine.isLiked(trackId);
}

bool PlaylistsController::toggleLike(const Core::Track track) {
  bool ok = m_engine.toggleLike(track);
  if (!ok)
    Q_EMIT errorOccurred();
  return ok;
}

bool PlaylistsController::updateSortOrder(const QString &playlistId, Core::TrackSortCriterion criterion, bool isReversed) {
  bool ok = m_engine.updateSortOrder(playlistId, criterion, isReversed);
  if (!ok)
    Q_EMIT errorOccurred();
  return ok;
}

QList<Core::Playlist> PlaylistsController::getPlaylists() const { return m_engine.getPlaylists(); }

Models::TrackListModel *PlaylistsController::getTracksFromPlaylist(const QString &playlistId) const {
  QList<Core::Track> tracks = m_engine.getTracksFromPlaylist(playlistId);
  Models::TrackListModel *tracksModel = new Models::TrackListModel;
  QQmlEngine::setObjectOwnership(tracksModel, QQmlEngine::JavaScriptOwnership);
  tracksModel->setTracks(tracks);
  return tracksModel;
}

Models::PlaylistsModel *PlaylistsController::getPlaylistsFromTrack(const QString &trackId) {
  QList<Core::Playlist> playlists = m_engine.getPlaylistsFromTrack(trackId);
  Models::PlaylistsModel *playlistsModel = new Models::PlaylistsModel;
  QQmlEngine::setObjectOwnership(playlistsModel, QQmlEngine::JavaScriptOwnership);
  playlistsModel->setPlaylists(playlists);
  return playlistsModel;
}

QList<Core::Playlist> PlaylistsController::getPlaylistsListFromTrack(const QString &playlistId) {
  return m_engine.getPlaylistsFromTrack(playlistId);
}

bool PlaylistsController::isTrackInPlaylist(const QString &trackId, const QString &playlistId) {
  return m_engine.isTrackInPlaylist(trackId, playlistId);
}

QList<Core::Track> PlaylistsController::getTracksListFromPlaylist(const QString &playlistId) const {
  return m_engine.getTracksFromPlaylist(playlistId);
}

Core::Playlist PlaylistsController::getPlaylistFromId(const QString &playlistId) const {
  return m_engine.getPlaylistFromId(playlistId);
}

QString PlaylistsController::getLastError() const { return m_engine.getLastError(); }
