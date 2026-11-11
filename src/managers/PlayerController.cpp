#include "PlayerController.h"
#include <QCoreApplication>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(PlayerController_l, "PlayerController")

PlayerController::PlayerController(QObject *parent) : QObject(parent) {
  workerThread = new QThread(this);
  worker = new PlayerEngine();
  worker->moveToThread(workerThread);

  connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);

  connect(
      this, &PlayerController::requestPlayTrackFromPlaylist, worker,
      [this](const Core::Track &track, const Core::Playlist &pl) { worker->playFromPlaylist(track, pl); },
      Qt::QueuedConnection);
  connect(
      this, &PlayerController::requestPlayPlaylist, worker,
      [this](const Core::Playlist &pl) { worker->playFromPlaylist(pl); }, Qt::QueuedConnection);
  connect(this, &PlayerController::requestPlay, worker, &PlayerEngine::play, Qt::QueuedConnection);
  connect(this, &PlayerController::requestPause, worker, &PlayerEngine::pause, Qt::QueuedConnection);
  connect(this, &PlayerController::requestResume, worker, &PlayerEngine::resume, Qt::QueuedConnection);
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

  connect(worker, &PlayerEngine::currentTrackChanged, this, [this]() { Q_EMIT currentTrackChanged(); });
  connect(worker, &PlayerEngine::currentPlaylistChanged, this, [this]() { Q_EMIT currentPlaylistChanged(); });

  connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [this]() {
    workerThread->quit();
    workerThread->wait();
  });

  workerThread->start();
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

Core::Track PlayerController::getCurrentTrack() { return worker->getCurrentTrack(); }

void PlayerController::play(const Core::Track &track) { Q_EMIT requestPlay(track); }
void PlayerController::pause() { Q_EMIT requestPause(); }
void PlayerController::resume() { Q_EMIT requestResume(); }
void PlayerController::next() { Q_EMIT requestNext(); }
void PlayerController::prev() { Q_EMIT requestPrev(); }
void PlayerController::seekTo(long positionMs) { Q_EMIT requestSeekTo(positionMs); }
void PlayerController::playTrackFromPlaylist(const Core::Track& track, const Core::Playlist& pl) {
  Q_EMIT requestPlayTrackFromPlaylist(track, pl);
}
void PlayerController::playPlaylist(const Core::Playlist& pl) { Q_EMIT requestPlayPlaylist(pl); }

PlayerController::PlayerState PlayerController::getPlayerState() { return cachedState; }
long PlayerController::getPosition() { return cachedPosition; }
long PlayerController::getDuration() { return cachedDuration; }

Core::Playlist PlayerController::getCurrentPlaylist() { return worker->getCurrentPlaylist(); }
