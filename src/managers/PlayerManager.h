#pragma once

#include "Player.h"
#include "PlayerState.h"
#include "YtManager.h"
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QThread>
#include <qtmetamacros.h>

class PlayerManager : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_NAMED_ELEMENT(Player)
  Q_PROPERTY(Core::PlayerState::State playerState READ getPlayerState NOTIFY playerStateChanged)

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
  PlayerState getPlayerState();

Q_SIGNALS:
  void playerStateChanged();
  void errorOccured(const QString &error);

private:
  explicit PlayerManager(QObject *parent = nullptr);
  QString currentUrl{};
  int currentTrackIndex{};
  QList<Core::Track> tracks{};
  std::unique_ptr<Core::Player> player;
  YtManager *YT;

  bool playAfterExtract{false};
};
