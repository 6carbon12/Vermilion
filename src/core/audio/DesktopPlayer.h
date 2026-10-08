#pragma once
#include "Player.h"
#include "PlayerState.h"
#include <QTimer>
#include <QMediaPlayer>

namespace Core {
class DesktopPlayer : public Player {
  Q_OBJECT
public:
  DesktopPlayer();
  ~DesktopPlayer() = default;

  void loadTrack(const QString &url, const Core::Track &track) override;
  void play() override;
  void pause() override;
  void seekTo(long positionMs) override;
  long getCurrentPosition() override;
  long getDuration() override;
  Core::PlayerState::State getPlayerState() override;

private:
  /// @brief Gives PlayerState from the java playerState.
  Core::PlayerState::State getPlayerStateFromInt(int playerState);
  QMediaPlayer* player;
  QTimer *positionPoolTimer;
  bool playAfterReady;
};
} // namespace Core
