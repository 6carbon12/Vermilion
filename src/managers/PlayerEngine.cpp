#include "PlayerEngine.h"
#include "YtController.h"
#include <QDebug>

PlayerEngine::PlayerEngine(QObject *parent) : QObject(parent) {
  player = Core::Player::create();
  YT = YtController::instance()->getCore();
  progressTimer = new QTimer(this);

  progressTimer->setInterval(200);

  connect(YT, &YtEngine::extractionSuccess, this, [this](const QString &filePath) {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (!player) {
      return;
    }
    player->setUrl(filePath);

    if (playAfterExtract) {
      playAfterExtract = false;
      player->play();
      Q_EMIT stateChanged(player->getPlayerState());
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

  connect(YT, &YtEngine::getRelatedTracksFailed, this,
          [this](const QString &error) { Q_EMIT errorOccurred(error); });

  connect(progressTimer, &QTimer::timeout, this, [this]() {
    if (player && player->getPlayerState() == PlayerState::Playing) {
      Q_EMIT positionChanged(player->getCurrentPosition(), player->getDuration());
    }
  });
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
    progressTimer->start();
    Q_EMIT stateChanged(player->getPlayerState());
  }
}

void PlayerEngine::pause() {
  if (player) {
    player->pause();
    progressTimer->stop();
    Q_EMIT stateChanged(player->getPlayerState());
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
