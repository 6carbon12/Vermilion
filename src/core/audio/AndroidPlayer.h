#pragma once
#include "Player.h"
#include "PlayerState.h"
#include <QJniObject>

namespace Core {
class AndroidPlayer : public Player {
public:
  AndroidPlayer();
  ~AndroidPlayer();

  void loadTrack(const QString &url, const Core::Track &track) override;
  void play() override;
  void pause() override;
  void seekTo(long positionMs) override;
  long getCurrentPosition() override;
  long getDuration() override;
  Core::PlayerState::State getPlayerState() override;

private:
  Core::PlayerState::State getPlayerStateFromInt(int playerState);
  QJniObject player;
};
} // namespace Core
