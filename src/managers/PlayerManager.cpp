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
    qCDebug(PlayerManager_l) << "Playing file: " << filePath;
    if (player) {
      player->play(filePath);
      Q_EMIT playerStateChanged();
    }
  });
  connect(YT, &YtManager::extractionFailed, this,
          [](const QString &error) { qDebug() << "Extraction failed" << error; });

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

void PlayerManager::play(const QString &url) { YT->requestExtraction(url); }

PlayerState PlayerManager::getPlayerState() { return player ? player->getPlayerState() : PlayerState::Error; }

void PlayerManager::pause() {
  if (player) {
    player->pause();
    Q_EMIT playerStateChanged();
  }
}

void PlayerManager::resume() {
  if (player) {
    player->resume();
    Q_EMIT playerStateChanged();
  }
}

