#include "PlayerEngine.h"
#include "Track.h"
#include "YtController.h"
#include <QDebug>

PlayerEngine::PlayerEngine(QObject *parent) : QObject(parent) {
  player = Core::Player::create();
  YT = YtController::instance()->getCore();

  connect(player.get(), &Core::Player::requestNext, this, [this]() { next(); });
  connect(player.get(), &Core::Player::requestPrev, this, [this]() { prev(); });
  connect(player.get(), &Core::Player::playerStateChanged, this,
          [this]() { Q_EMIT playerStateChanged(player->getPlayerState()); });
  connect(player.get(), &Core::Player::playerPositionChanged, this, [this]() {
    Q_EMIT positionChanged(player->getCurrentPosition(), player->getDuration());
  });

  connect(YT, &YtEngine::extractionSuccess, this, [this](const QString &filePath) {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (!player) {
      return;
    }
    Core::Track currentTrack = tracks[currentTrackIndex];
    player->loadTrack(filePath, currentTrack);

    if (playAfterExtract) {
      playAfterExtract = false;
      player->play();
    }
  });

  connect(YT, &YtEngine::extractionFailed, this, [this](const QString &error) {
    playAfterExtract = false;
    Q_EMIT errorOccurred("Extraction failed: " + error);
  });

  connect(YT, &YtEngine::getRelatedTracksSuccess, this, [this](const QList<Core::Track> &relatedTracks) {
    std::lock_guard<std::mutex> lock(queueMutex);
    tracks = relatedTracks;
  });

  connect(YT, &YtEngine::getRelatedTracksFailed, this, [this](const QString &error) { Q_EMIT errorOccurred(error); });

}

void PlayerEngine::setUrl(const QString &url) {
  std::lock_guard<std::mutex> lock(queueMutex);
  currentUrl = url;
  currentTrackIndex = 0;
  tracks.clear();
  YT->requestExtraction(url);
  YT->getRelatedTracks(url);
}

void PlayerEngine::play() {
  if (player) {
    player->play();
  }
}

void PlayerEngine::pause() {
  if (player) {
    player->pause();
  }
}

void PlayerEngine::next() {
  std::lock_guard<std::mutex> lock(queueMutex);
  if (currentTrackIndex >= tracks.length() - 1) {
    Q_EMIT errorOccurred("Cannot go next, reached end of tracks.");
    return;
  }
  currentTrackIndex++;
  currentUrl = tracks[currentTrackIndex].url;
  playAfterExtract = true;
  YT->requestExtraction(currentUrl);
}

void PlayerEngine::prev() {
  std::lock_guard<std::mutex> lock(queueMutex);
  if (currentTrackIndex <= 0) {
    Q_EMIT errorOccurred("Cannot go to previous track.");
    return;
  }
  currentTrackIndex--;
  currentUrl = tracks[currentTrackIndex].url;
  playAfterExtract = true;
  YT->requestExtraction(currentUrl);
}

void PlayerEngine::seekTo(long positionMs) {
  if (player) {
    player->seekTo(positionMs);
  }
}
