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

void PlayerManager::setUrl(const QString &url) {
  currentUrl = url;
  YT->requestExtraction(url);
}

void PlayerManager::play() {
  player->play();
  Q_EMIT playerStateChanged();
}

PlayerState PlayerManager::getPlayerState() { return player ? player->getPlayerState() : PlayerState::Error; }

void PlayerManager::pause() {
  player->pause();
  Q_EMIT playerStateChanged();
}
