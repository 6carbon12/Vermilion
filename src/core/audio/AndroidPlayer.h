#pragma once
#include "Player.h"
#include "PlayerState.h"
#include <QJniObject>
#include <QTimer>

namespace Core {
class AndroidPlayer : public Player {
  Q_OBJECT
public:
  AndroidPlayer();
  ~AndroidPlayer();

  void play(const QString &url, const Core::Track &track) override;
  void pause() override;
  void resume() override;
  void seekTo(long positionMs) override;
  long getCurrentPosition() override;
  long getDuration() override;
  Core::PlayerState::State getPlayerState() override;

  Q_INVOKABLE void emitRequestNext();
  Q_INVOKABLE void emitRequestPrev();
  Q_INVOKABLE void handlePlayerStateChanged();
private:
  /// @brief Gives PlayerState from the java playerState.
  Core::PlayerState::State getPlayerStateFromInt(int playerState);
  QJniObject player; ///< Holds the actual Java player object.
  QTimer *positionPoolTimer;
};
} // namespace Core
