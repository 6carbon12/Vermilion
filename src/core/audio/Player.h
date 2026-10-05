#pragma once
#include "PlayerState.h"
#include "Track.h"
#include <QDir>
#include <memory>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace Core {

/// @brief Abstract base interface for media playback.
///
/// The `Player` class handles loading, playing, and seeking tracks.
/// It acts as a platform-agnostic interface, relying on the `create()`
/// factory method to instantiate the correct platform-specific backend.
class Player : public QObject {
  Q_OBJECT

public:
  using PlayerState = Core::PlayerState::State;
  explicit Player(QObject *parent = nullptr) : QObject(parent) {};
  ~Player();

  /// @brief Creates the appropriate player based on the current platform.
  /// @retval std::unique_ptr<Player> The newly created player instance.
  /// @retval nullptr If a player instance already exists.
  static std::unique_ptr<Player> create();

  /// @note For internal use by class implementers only.
  /// @return Normal pointer to current player instance.
  static Player* instance();

  /// @brief Loads the given track information and prepares the player.
  /// @note This function does not play the track, see `play()`.
  /// @param url File path of the track to be played.
  /// @param track Metadata of the given track.
  virtual void loadTrack(const QString &url, const Core::Track &track) = 0;

  /// @brief Plays the track loaded from `loadTrack`.
  virtual void play() = 0;

  /// @brief Pauses the track.
  virtual void pause() = 0;

  /// @param positionMs Position to seek to in milliseconds.
  virtual void seekTo(long positionMs) = 0;

  /// @return Track's current position in milliseconds.
  virtual long getCurrentPosition() = 0;

  /// @return Track's total duration in milliseconds.
  virtual long getDuration() = 0;

  /// @return Player's current state.
  virtual PlayerState getPlayerState() = 0;

Q_SIGNALS:
  /// @brief Emitted when the player wants the next track.
  void requestNext();

  /// @brief Emitted when the player wants the previous track.
  void requestPrev();

  /// @brief Emitted whenever the active track's playback state changes.
  void playerStateChanged();

  /// @brief Emitted when player's position is updated.
  ///
  /// Emitted every 200ms when player is actively playing.
  /// @see `getCurrentPosition`.
  void playerPositionChanged();
protected:
  PlayerState state{PlayerState::Initialized}; ///< Current operational state of the player.
  static Player* self; ///< Raw pointer to the current instance of the player.
};
} // namespace Core
