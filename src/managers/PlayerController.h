#pragma once

#include "PlayerState.h"
#include "PlayerEngine.h"
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QThread>

/// @brief Transparent singleton wrapper around `PlayerEngine`
///
/// Manages `PlayerEngine` in a separate thread and creates binding for it on main thread for QML.
/// Has a private constructor to force QML engine to use `create` function when initializing this object.
class PlayerController : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_NAMED_ELEMENT(Player)
  Q_PROPERTY(Core::PlayerState::State playerState READ getPlayerState NOTIFY playerStateChanged)
  Q_PROPERTY(long position READ getPosition NOTIFY positionChanged)
  Q_PROPERTY(long duration READ getDuration NOTIFY durationChanged)

public:
  using PlayerState = Core::PlayerState::State;

  ~PlayerController();
  static PlayerController *instance();
  static PlayerController *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

  Q_INVOKABLE void setUrl(const QString &url);
  Q_INVOKABLE void play();
  Q_INVOKABLE void pause();
  Q_INVOKABLE void next();
  Q_INVOKABLE void prev();
  Q_INVOKABLE void seekTo(long positionMs);
  Q_INVOKABLE long getPosition();
  Q_INVOKABLE long getDuration();
  PlayerState getPlayerState();

Q_SIGNALS:
  void playerStateChanged();
  void positionChanged();
  void durationChanged();
  void errorOccured(const QString &error);

  void requestSetUrl(const QString &url);
  void requestPlay();
  void requestPause();
  void requestNext();
  void requestPrev();
  void requestSeekTo(long positionMs);

private:
  explicit PlayerController(QObject *parent = nullptr);

  QThread *workerThread;
  PlayerEngine *worker;

  PlayerState cachedState{PlayerState::Initialized};
  long cachedPosition{0};
  long cachedDuration{0};
};
;
