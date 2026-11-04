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
  // TODO: Find better way to have consistent id for special playlists.
  currentPlaylist.id = "current";
  currentPlaylist.name = "Current Mix";
  currentPlaylist.type = Core::PlaylistType::Current;

  connect(player.get(), &Core::Player::requestNext, this, [this]() { next(); });
  connect(player.get(), &Core::Player::requestPrev, this, [this]() { prev(); });
  connect(player.get(), &Core::Player::playerStateChanged, this,
          [this]() { Q_EMIT playerStateChanged(player->getPlayerState()); });
  connect(player.get(), &Core::Player::playerPositionChanged, this,
          [this]() { Q_EMIT positionChanged(player->getCurrentPosition(), player->getDuration()); });

  connect(YT, &YtEngine::extractionSuccess, this, [this](const QString &extractedUrl) {
    std::lock_guard<std::mutex> lock(queueMutex);

    if (waitingForRelatedTracks) {
      pendingExtractionUrl = extractedUrl;
      return;
    }

    processExtraction(extractedUrl);
  });

  connect(YT, &YtEngine::extractionFailed, this,
          [this](const QString &error) { Q_EMIT errorOccurred("Extraction failed: " + error); });

  connect(YT, &YtEngine::getRelatedTracksSuccess, this, &PlayerEngine::handleRelatedTracks);
  connect(YT, &YtEngine::getRelatedTracksFailed, this, [this](const QString &error) {
    waitingForRelatedTracks = false;
    Q_EMIT errorOccurred(error);
  });
}

void PlayerEngine::play(const QString &url) {
  std::lock_guard<std::mutex> lock(queueMutex);
  currentTrackIndex = 0;
  tracks.clear();

  waitingForRelatedTracks = true;
  pendingExtractionUrl.clear();

  YT->getRelatedTracks(url);
  YT->requestExtraction(url);
}

void PlayerEngine::playFromPlaylist(const Core::Track track, const Core::Playlist playlist) {
  updateCurrentPlaylist(plctl->getTracksListFromPlaylist(playlist.id));
  int idx = tracks.indexOf(track);
  if (idx >= 0) {
    currentTrackIndex = idx;
  }
  YT->requestExtraction(tracks[currentTrackIndex].url);
}

void PlayerEngine::playFromPlaylist(const Core::Playlist playlist) {
  waitingForRelatedTracks = false;
  QList<Core::Track> newTracks = plctl->getTracksListFromPlaylist(playlist.id);
  if (newTracks.isEmpty()) {
    return;
  }
  updateCurrentPlaylist(plctl->getTracksListFromPlaylist(playlist.id));
  YT->requestExtraction(tracks[currentTrackIndex].url);
}

void PlayerEngine::pause() {
  if (player) {
    player->pause();
  }
}

void PlayerEngine::resume() {
  if (player) {
    player->resume();
  }
}

void PlayerEngine::next() {
  std::lock_guard<std::mutex> lock(queueMutex);
  if (currentTrackIndex >= tracks.length() - 1) {
    Q_EMIT errorOccurred("Cannot go next, reached end of tracks.");
    return;
  }
  currentTrackIndex++;
  YT->requestExtraction(tracks[currentTrackIndex].url);
}

void PlayerEngine::prev() {
  std::lock_guard<std::mutex> lock(queueMutex);
  if (currentTrackIndex <= 0) {
    Q_EMIT errorOccurred("Cannot go to previous track.");
    return;
  }
  currentTrackIndex--;
  YT->requestExtraction(tracks[currentTrackIndex].url);
}

void PlayerEngine::seekTo(long positionMs) {
  if (player) {
    player->seekTo(positionMs);
  }
}

void PlayerEngine::handleRelatedTracks(const QList<Core::Track> &relatedTracks) {
  updateCurrentPlaylist(relatedTracks);
  waitingForRelatedTracks = false;
  if (!pendingExtractionUrl.isEmpty()) {
    processExtraction(pendingExtractionUrl);
    pendingExtractionUrl.clear();
  }
}

void PlayerEngine::updateCurrentPlaylist(const QList<Core::Track> &relatedTracks) {
  std::lock_guard<std::mutex> lock(queueMutex);
  currentTrackIndex = 0;

  plctl->deletePlaylist(currentPlaylist.id);
  plctl->createPlaylist(currentPlaylist);
  plctl->addToPlaylist(relatedTracks, currentPlaylist.id);

  tracks.clear();
  tracks = plctl->getTracksListFromPlaylist(currentPlaylist.id);

  Q_EMIT currentPlaylistChanged();
}

void PlayerEngine::processExtraction(const QString &extractedUrl) {
  if (!player) {
    return;
  }

  // Safety bound check to prevent crashes if tracks are somehow empty
  if (tracks.isEmpty() || currentTrackIndex < 0 || currentTrackIndex >= tracks.length()) {
    Q_EMIT errorOccurred("Queue error: No track information available.");
    return;
  }

  Core::Track currentTrack = tracks[currentTrackIndex];
  player->play(extractedUrl, currentTrack);
  Q_EMIT currentTrackChanged();
}

Core::Track PlayerEngine::getCurrentTrack() {
  if (!tracks.isEmpty()) {
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
