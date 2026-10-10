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

  connect(YT, &YtEngine::extractionFailed, this, [this](const QString &error) {
    Q_EMIT errorOccurred("Extraction failed: " + error);
  });

  connect(YT, &YtEngine::getRelatedTracksSuccess, this, [this](const QList<Core::Track> &relatedTracks) {
    std::lock_guard<std::mutex> lock(queueMutex);
    currentTrackIndex = 0;
    tracks = relatedTracks;

    waitingForRelatedTracks = false;
    if (!pendingExtractionUrl.isEmpty()) {
      processExtraction(pendingExtractionUrl);
      pendingExtractionUrl.clear();
    }
  });

  connect(YT, &YtEngine::getRelatedTracksFailed, this, [this](const QString &error) { 
      waitingForRelatedTracks = false;
      Q_EMIT errorOccurred(error);
      });
}

void PlayerEngine::play(const QString &url) {
  std::lock_guard<std::mutex> lock(queueMutex);
  currentUrl = url;
  currentTrackIndex = 0;
  tracks.clear();

  waitingForRelatedTracks = true;
  pendingExtractionUrl.clear();

  YT->getRelatedTracks(url);
  YT->requestExtraction(url);
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
  currentUrl = tracks[currentTrackIndex].url;
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
  YT->requestExtraction(currentUrl);
}

void PlayerEngine::seekTo(long positionMs) {
  if (player) {
    player->seekTo(positionMs);
  }
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
}
