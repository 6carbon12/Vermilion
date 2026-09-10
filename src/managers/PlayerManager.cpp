#include "PlayerManager.h"
#include <qloggingcategory.h>
#include <qtmetamacros.h>
#include <qtpreprocessorsupport.h>

Q_LOGGING_CATEGORY(PlayerManager_l, "PlayerManager")
using PlayerState = Core::PlayerState::State;

PlayerManager::PlayerManager(QObject *parent) : QObject(parent) {
  qCDebug(PlayerManager_l) << "Called private constructor.";
  player = Core::Player::create();
  YT = YtManager::instance();

  connect(YT, &YtManager::extractionSuccess, this, [this](const QString &filePath) {
    player->setUrl(filePath);
    if (playAfterExtract) {
      playAfterExtract = false;
      player->play();
    }
  });
  connect(YT, &YtManager::extractionFailed, this, [this](const QString &error) {
    playAfterExtract = false;
    qDebug() << "Extraction failed" << error;
  });

  connect(YT, &YtManager::getRelatedTracksSuccess, this,
          [this](const QList<Core::Track> &realtedTracks) { tracks = realtedTracks; });
  connect(YT, &YtManager::getRelatedTracksFailed, this, [this](const QString &error) { Q_EMIT errorOccured(error); });

  qCDebug(PlayerManager_l) << "Initalization complete.";
}

PlayerManager *PlayerManager::instance() {
  static PlayerManager _instance;
  return &_instance;
}

PlayerManager *PlayerManager::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
  Q_UNUSED(qmlEngine);
  Q_UNUSED(jsEngine);
  return PlayerManager::instance();
}

void PlayerManager::setUrl(const QString &url) {
  currentUrl = url;
  currentTrackIndex = 0;
  YT->requestExtraction(url);
  YT->getRelatedTracks(url);
}

void PlayerManager::play() {
  player->play();
  Q_EMIT playerStateChanged();
}

void PlayerManager::pause() {
  player->pause();
  Q_EMIT playerStateChanged();
}

PlayerState PlayerManager::getPlayerState() { return player ? player->getPlayerState() : PlayerState::Error; }

void PlayerManager::next() {
  if (currentTrackIndex >= tracks.length()) {
    qCWarning(PlayerManager_l) << "Cannot go to next track. No next track present.";
    qCWarning(PlayerManager_l) << "Tracks: " << tracks.length();
    Q_EMIT errorOccured("Cannot go next, reached end of tracks.");
    return;
  }

  currentTrackIndex++;
  currentUrl = tracks[currentTrackIndex].url;
  playAfterExtract = true;
  YT->requestExtraction(currentUrl);
}

void PlayerManager::prev() {
  if (currentTrackIndex == 0) {
    qCWarning(PlayerManager_l) << "Cannot go to previous track. No previous track present.";
    Q_EMIT errorOccured("Cannot go to previous track. No previous track present.");
    return;
  }

  currentTrackIndex--;
  currentUrl = tracks[currentTrackIndex].url;
  playAfterExtract = true;
  YT->requestExtraction(currentUrl);
}
