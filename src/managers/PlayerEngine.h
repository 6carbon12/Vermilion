#pragma once

#include "Player.h"
#include "PlayerState.h"
#include "YtEngine.h"
#include <QList>
#include <QObject>
#include <QString>
#include <QTimer>
#include <mutex>

/// @brief Manages Player and It's Queue.
class PlayerEngine : public QObject {
  Q_OBJECT
public:
  using PlayerState = Core::PlayerState::State;
  explicit PlayerEngine(QObject *parent = nullptr);
  ~PlayerEngine() = default;

public Q_SLOTS:
  /// @brief Prepares the player to play `url`.
  /// @param url YouTube URL of song to prepare.
  void setUrl(const QString &url);

  /// @brief Plays the track.
  /// @note Track must be set before with `setUrl`.
  void play();

  /// @brief Pauses the track.
  void pause();

  /// @brief Moves to next track in the queue.
  void next();

  /// @brief Moves to previous track in the queue.
  void prev();

  /// @brief Seeks to `positionMs` in the track given in milliseconds.
  void seekTo(long positionMs);

Q_SIGNALS:
  /// @brief Emitted when player state changes.
  /// @param state Current state of player.
  void playerStateChanged(Core::PlayerState::State state);

  /// @brief Emitted when player's position is changed
  /// @param position Indicates current position in milliseconds.
  /// @param duration Indicates total duration of the track in milliseconds.
  /// @note This signal is triggered every 200ms when player is actively playing a track.
  void positionChanged(long position, long duration);

  /// @brief Emitted whenever any error occurs.
  /// @param error Human readable error message.
  void errorOccurred(const QString &error);

private:
  bool playAfterExtract{false};
  int currentTrackIndex{};
  QList<Core::Track> tracks{};
  QString currentUrl{};
  std::mutex queueMutex;
  std::unique_ptr<Core::Player> player;
  YtEngine *YT;
};
