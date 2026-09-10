#pragma once
#include "Player.h"
#include "PlayerState.h"
#include <QJniObject>

namespace Core {
class AndroidPlayer : public Player {
public:
  AndroidPlayer();
  ~AndroidPlayer();

  void play(const QString &filePath) override;
  void pause() override;
  void resume() override;
  Core::PlayerState::State getPlayerState() override;

private:
  Core::PlayerState::State getPlayerStateFromInt(int playerState);
  QJniObject player;
};
} // namespace Core
