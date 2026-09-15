#pragma once

#include "Player.h"
#include "PlayerState.h"
#include "YtEngine.h"
#include <QList>
#include <QObject>
#include <QString>
#include <QTimer>
#include <mutex>

class PlayerEngine : public QObject {
  Q_OBJECT
public:
  using PlayerState = Core::PlayerState::State;
  explicit PlayerEngine(QObject *parent = nullptr);
  ~PlayerEngine() = default;

public Q_SLOTS:
  void setUrl(const QString &url);
  void play();
  void pause();
  void next();
  void prev();
  void seekTo(long positionMs);

Q_SIGNALS:
  void stateChanged(Core::PlayerState::State state);
  void positionChanged(long position, long duration);
  void errorOccurred(const QString &error);

private:
  bool playAfterExtract{false};
  int currentTrackIndex{};
  QList<Core::Track> tracks{};
  QString currentUrl{};
  QTimer *progressTimer;
  std::mutex queueMutex;
  std::unique_ptr<Core::Player> player;
  YtEngine *YT;
};
