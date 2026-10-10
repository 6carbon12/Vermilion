#pragma once
#include "Player.h"
#include "PlayerState.h"
#include <QTimer>
#include <QMediaPlayer>
#include <mpv/client.h>

class MprisController;
namespace Core {
class DesktopPlayer : public Player {
  Q_OBJECT
public:
  DesktopPlayer();
  ~DesktopPlayer() = default;

  void play(const QString &url, const Core::Track &track) override;
  void pause() override;
  void resume() override;
  void seekTo(long positionMs) override;
  long getCurrentPosition() override;
  long getDuration() override;
  Core::PlayerState::State getPlayerState() override;

  // For DBus
  void next();
  void prev();
  Core::Track& getCurrentTrack();
  Q_SIGNAL void trackChanged();
  Q_SIGNAL void seeked();

private:
  void handleMpvState(mpv_handle *handle);
  mpv_handle *mpvHandle;
  long position;
  long duration;
  Core::Track currentTrack;
  MprisController* mprisController;
};
} // namespace Core
