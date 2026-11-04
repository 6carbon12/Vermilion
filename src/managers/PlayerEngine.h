#pragma once

#include "Player.h"
#include "PlayerState.h"
#include "Playlist.h"
#include "PlaylistController.h"
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
  /// @brief Plays the given url.
  /// @param url YouTube URL of song to prepare.
  void play(const QString &url);

  // TODO: Report empty/invalid playlists somehow.

  /// @brief Plays a given track from a given playlist.
  /// @param track Track object of the song that needs to be played.
  /// @param playlist Playlist object of the playlist that `track` is a part of.
  ///
  /// If `track` is not found in `playlist`, first song from playlist is played.
  /// If `playlist` doesn't exit, nothing happens.
  void playFromPlaylist(const Core::Track track, const Core::Playlist playlist);

  /// @brief Starts playing the given playlist.
  /// @param playlist Playlist object of the playlist to be played.
  ///
  /// If `playlist` doesn't exit, nothing happens.
  void playFromPlaylist(const Core::Playlist playlist);

  /// @brief Resumes playback.
  void resume();

  /// @brief Pauses playback.
  void pause();

  /// @brief Moves to next track in the queue.
  void next();

  /// @brief Moves to previous track in the queue.
  void prev();

  /// @brief Seeks to `positionMs` in the track given in milliseconds.
  void seekTo(long positionMs);

  /// @brief Gives currently playing track's info.
  /// @see `currentTrackChanged`.
  Core::Track getCurrentTrack();

  Core::Playlist getCurrentPlaylist();

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

  /// @brief Emitted when currently playing track is changed.
  void currentTrackChanged();

  /// @brief Emitted when currently playing playlist is changed.
  void currentPlaylistChanged();

private:
  int currentTrackIndex{};
  QList<Core::Track> tracks{};
  Core::Playlist currentPlaylist;
  std::mutex queueMutex;
  std::unique_ptr<Core::Player> player;
  YtEngine *YT;
  PlaylistsController* plctl{PlaylistsController::instance()};

  QString pendingExtractionUrl;
  bool waitingForRelatedTracks = false;
  void processExtraction(const QString &extractedUrl);
  void handleRelatedTracks(const QList<Core::Track> &relatedTracks);
  void updateCurrentPlaylist(const QList<Core::Track> &relatedTracks);
};
