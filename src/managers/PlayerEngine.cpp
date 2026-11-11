#include "PlayerEngine.h"
#include "Playlist.h"
#include "PlaylistController.h"
#include "Track.h"
#include "YtController.h"
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(PlayerEngine_l, "PlayerEngine")

PlayerEngine::PlayerEngine(QObject *parent) : QObject(parent) {
  player = Core::Player::create();
  YT = YtController::instance()->getCore();

  currentPlaylist.id = "current";
  currentPlaylist.name = "Current Mix";
  currentPlaylist.type = Core::PlaylistType::Current;

  connect(player.get(), &Core::Player::requestNext, this, &PlayerEngine::next);
  connect(player.get(), &Core::Player::requestPrev, this, &PlayerEngine::prev);
  connect(player.get(), &Core::Player::playerStateChanged, this,
          [this]() { Q_EMIT playerStateChanged(player->getPlayerState()); });
  connect(player.get(), &Core::Player::playerPositionChanged, this,
          [this]() { Q_EMIT positionChanged(player->getCurrentPosition(), player->getDuration()); });

  connect(YT, &YtEngine::extractionSuccess, this, [this](const QString &extractedUrl) {
    std::lock_guard<std::mutex> lock(queueMutex);
    processExtraction(extractedUrl);
  });

  connect(YT, &YtEngine::extractionFailed, this, [this](const QString &error) {
    Q_EMIT playerStateChanged(PlayerState::Initialized);
    Q_EMIT errorOccurred("Extraction failed: " + error);
  });

  connect(YT, &YtEngine::getRelatedTracksSuccess, this, &PlayerEngine::handleRelatedTracks);
  connect(YT, &YtEngine::getRelatedTracksFailed, this,
          [this](const QString &error) { Q_EMIT errorOccurred("Failed to load related tracks: " + error); });
}

void PlayerEngine::play(const Core::Track &track) {
  player->stop();
  Q_EMIT playerStateChanged(PlayerState::Buffering);

  {
    std::lock_guard<std::mutex> lock(queueMutex);
    tracks.clear();
    tracks.append(track);
    currentTrackIndex = 0;
  }

  Q_EMIT currentTrackChanged();

  YT->requestExtraction(track.url);
  YT->getRelatedTracks(track.url);
}

void PlayerEngine::playFromPlaylist(const Core::Track &track, const Core::Playlist &playlist) {
  player->stop();
  Q_EMIT playerStateChanged(PlayerState::Buffering);

  updateCurrentPlaylist(plctl->getTracksListFromPlaylist(playlist.id));

  std::lock_guard<std::mutex> lock(queueMutex);

  int idx = tracks.indexOf(track);
  currentTrackIndex = (idx >= 0) ? idx : 0;

  YT->requestExtraction(tracks[currentTrackIndex].url);
}

void PlayerEngine::playFromPlaylist(const Core::Playlist &playlist) {
  player->stop();
  Q_EMIT playerStateChanged(PlayerState::Buffering);

  QList<Core::Track> newTracks = plctl->getTracksListFromPlaylist(playlist.id);
  if (newTracks.isEmpty()) {
    Q_EMIT playerStateChanged(PlayerState::Initialized);
    return;
  }

  updateCurrentPlaylist(newTracks);

  std::lock_guard<std::mutex> lock(queueMutex);
  currentTrackIndex = 0;

  YT->requestExtraction(tracks[currentTrackIndex].url);
}

void PlayerEngine::pause() {
  if (player)
    player->pause();
}

void PlayerEngine::resume() {
  if (player)
    player->resume();
}

void PlayerEngine::next() {
  player->stop();
  std::lock_guard<std::mutex> lock(queueMutex);
  if (currentTrackIndex >= tracks.length() - 1) {
    Q_EMIT errorOccurred("Cannot go next, reached end of tracks.");
    return;
  }

  Q_EMIT playerStateChanged(PlayerState::Buffering);
  currentTrackIndex++;
  YT->requestExtraction(tracks[currentTrackIndex].url);
}

void PlayerEngine::prev() {
  player->stop();
  std::lock_guard<std::mutex> lock(queueMutex);
  if (currentTrackIndex <= 0) {
    Q_EMIT errorOccurred("Cannot go to previous track.");
    return;
  }

  Q_EMIT playerStateChanged(PlayerState::Buffering);
  currentTrackIndex--;
  YT->requestExtraction(tracks[currentTrackIndex].url);
}

void PlayerEngine::seekTo(long positionMs) {
  if (player)
    player->seekTo(positionMs);
}

void PlayerEngine::handleRelatedTracks(const QList<Core::Track> &relatedTracks) {
  QString currentId;
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (!tracks.isEmpty() && currentTrackIndex >= 0 && currentTrackIndex < tracks.length()) {
      currentId = tracks[currentTrackIndex].id;
    }
  }

  updateCurrentPlaylist(relatedTracks);

  {
    std::lock_guard<std::mutex> lock(queueMutex);
    currentTrackIndex = 0; // Fallback
    for (int i = 0; i < tracks.size(); ++i) {
      if (tracks[i].id == currentId) {
        currentTrackIndex = i;
        break;
      }
    }
  }
}

void PlayerEngine::updateCurrentPlaylist(const QList<Core::Track> &relatedTracks) {
  plctl->blockSignals(true);
  plctl->deletePlaylist(currentPlaylist.id);
  plctl->createPlaylist(currentPlaylist);
  plctl->addToPlaylist(relatedTracks, currentPlaylist.id);
  plctl->blockSignals(false);

  {
    std::lock_guard<std::mutex> lock(queueMutex);
    tracks = plctl->getTracksListFromPlaylist(currentPlaylist.id);
  }

  Q_EMIT currentPlaylistChanged();
  Q_EMIT plctl->playlistUpdated(currentPlaylist.id);
  Q_EMIT plctl->tracksUpdated(currentPlaylist.id);
}

void PlayerEngine::processExtraction(const QString &extractedUrl) {
  if (!player)
    return;

  if (tracks.isEmpty() || currentTrackIndex < 0 || currentTrackIndex >= tracks.length()) {
    Q_EMIT playerStateChanged(PlayerState::Initialized);
    Q_EMIT errorOccurred("Queue error: No track information available.");
    return;
  }

  player->play(extractedUrl, tracks[currentTrackIndex]);
  Q_EMIT currentTrackChanged();
}

Core::Track PlayerEngine::getCurrentTrack() {
  std::lock_guard<std::mutex> lock(queueMutex);

  if (!tracks.isEmpty() && currentTrackIndex >= 0 && currentTrackIndex < tracks.length()) {
    return tracks[currentTrackIndex];
  }

  Core::Track track;
  track.views = "0";
  track.coverArtUrl = "qrc:/icons/default-playlist.svg";
  track.durationString = "00:00";
  track.duration = 0;
  track.url = "N/A";
  track.title = "N/A";
  track.isExplict = false;
  track.id = "undefined";
  track.artistsNum = 0;

  return track;
}

Core::Playlist PlayerEngine::getCurrentPlaylist() { return currentPlaylist; }
