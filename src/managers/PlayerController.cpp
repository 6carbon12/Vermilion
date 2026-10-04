#include "PlayerController.h"
#include <QCoreApplication>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(PlayerManager_l, "PlayerManager")

PlayerController::PlayerController(QObject *parent) : QObject(parent) {
  qCDebug(PlayerManager_l) << "Initializing PlayerManager wrapper.";

  workerThread = new QThread(this);
  worker = new PlayerEngine();
  worker->moveToThread(workerThread);

  connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);

  connect(this, &PlayerController::requestSetUrl, worker, &PlayerEngine::setUrl, Qt::QueuedConnection);
  connect(this, &PlayerController::requestPlay, worker, &PlayerEngine::play, Qt::QueuedConnection);
  connect(this, &PlayerController::requestPause, worker, &PlayerEngine::pause, Qt::QueuedConnection);
  connect(this, &PlayerController::requestNext, worker, &PlayerEngine::next, Qt::QueuedConnection);
  connect(this, &PlayerController::requestPrev, worker, &PlayerEngine::prev, Qt::QueuedConnection);
  connect(this, &PlayerController::requestSeekTo, worker, &PlayerEngine::seekTo, Qt::QueuedConnection);

  connect(worker, &PlayerEngine::playerStateChanged, this, [this](PlayerState newState) {
    cachedState = newState;
    Q_EMIT playerStateChanged();
  });

  connect(worker, &PlayerEngine::positionChanged, this, [this](long pos, long dur) {
    cachedPosition = pos;
    cachedDuration = dur;
    Q_EMIT positionChanged();
    Q_EMIT durationChanged();
  });

  connect(worker, &PlayerEngine::errorOccurred, this, [this](const QString &err) { Q_EMIT errorOccured(err); });

  connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [this]() {
    workerThread->quit();
    workerThread->wait();
  });

  workerThread->start();
  qCDebug(PlayerManager_l) << "PlayerManager background thread started.";
}

PlayerController::~PlayerController() = default;

PlayerController *PlayerController::instance() {
  static PlayerController _instance;
  return &_instance;
}

PlayerController *PlayerController::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
  Q_UNUSED(qmlEngine);
  Q_UNUSED(jsEngine);
  PlayerController *inst = PlayerController::instance();
  QQmlEngine::setObjectOwnership(inst, QQmlEngine::CppOwnership);
  return inst;
}

void PlayerController::setUrl(const QString &url) { Q_EMIT requestSetUrl(url); }
void PlayerController::play() { Q_EMIT requestPlay(); }
void PlayerController::pause() { Q_EMIT requestPause(); }
void PlayerController::next() { Q_EMIT requestNext(); }
void PlayerController::prev() { Q_EMIT requestPrev(); }
void PlayerController::seekTo(long positionMs) { Q_EMIT requestSeekTo(positionMs); }

PlayerController::PlayerState PlayerController::getPlayerState() { return cachedState; }
long PlayerController::getPosition() { return cachedPosition; }
long PlayerController::getDuration() { return cachedDuration; }
