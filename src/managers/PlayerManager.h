#pragma once

#include "Player.h"
#include "PlayerState.h"
#include "YtManager.h"
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QThread>
#include <QTimer>
#include <qtmetamacros.h>
#include <mutex>


class PlayerManager : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_NAMED_ELEMENT(Player)
  Q_PROPERTY(Core::PlayerState::State playerState READ getPlayerState NOTIFY playerStateChanged)
  Q_PROPERTY(long position READ getPosition NOTIFY positionChanged)
  Q_PROPERTY(long duration READ getDuration NOTIFY durationChanged)

public:
  using PlayerState = Core::PlayerState::State;

  ~PlayerManager() = default;
  static PlayerManager *instance();
  static PlayerManager *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

  Q_INVOKABLE void setUrl(const QString &url);
  Q_INVOKABLE void play();
  Q_INVOKABLE void pause();
  Q_INVOKABLE void next();
  Q_INVOKABLE void prev();
  Q_INVOKABLE void seekTo(long postionMs);
  Q_INVOKABLE long getPosition();
  Q_INVOKABLE long getDuration();
  PlayerState getPlayerState();

Q_SIGNALS:
  void playerStateChanged();
  void positionChanged();
  void durationChanged();
  void errorOccured(const QString &error);

private:
  explicit PlayerManager(QObject *parent = nullptr);
  bool playAfterExtract{false};
  int currentTrackIndex{};
  QList<Core::Track> tracks{};
  QString currentUrl{};
  QTimer *progressTimer;
  std::mutex queueMutex;
  std::unique_ptr<Core::Player> player;
  YtManager *YT;
};
