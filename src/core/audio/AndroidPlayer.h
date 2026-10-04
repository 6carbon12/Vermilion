#pragma once
#include "Player.h"
#include "PlayerState.h"
#include <QJniObject>

namespace Core {
class AndroidPlayer : public Player {
  Q_OBJECT
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

  Q_INVOKABLE void emitRequestNext();
  Q_INVOKABLE void emitRequestPrev();
  Q_INVOKABLE void handlePlayerStateChanged();
private:
  Core::PlayerState::State getPlayerStateFromInt(int playerState);
  QJniObject player;
};
} // namespace Core
