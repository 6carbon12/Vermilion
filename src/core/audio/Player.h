#pragma once
#include "PlayerState.h"
#include "Track.h"
#include <QDir>
#include <memory>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace Core {

class Player : public QObject {
  Q_OBJECT

public:
  using PlayerState = Core::PlayerState::State;
  explicit Player(QObject *parent = nullptr) : QObject(parent) {};
  ~Player() = default;

  static std::unique_ptr<Player> create();
  virtual void loadTrack(const QString &url, const Core::Track &track) = 0;
  virtual void play() = 0;
  virtual void pause() = 0;
  virtual void seekTo(long positionMs) = 0;
  virtual long getCurrentPosition() = 0;
  virtual long getDuration() = 0;
  virtual PlayerState getPlayerState() = 0;

protected:
  PlayerState state{PlayerState::Initialized};
};
} // namespace Core
