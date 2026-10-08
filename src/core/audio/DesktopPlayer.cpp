#include "DesktopPlayer.h"
#include <QAudioOutput>
#include <QThread>
#include <qmediaplayer.h>

namespace Core {
DesktopPlayer::DesktopPlayer() {
  positionPoolTimer = new QTimer(this);
  positionPoolTimer->setInterval(200);
  player = new QMediaPlayer(this);
  player->setAudioOutput(new QAudioOutput(this));

  connect(positionPoolTimer, &QTimer::timeout, this, [this]() {
    if (state == PlayerState::Playing) {
      Q_EMIT playerPositionChanged();
    }
  });

  connect(player, &QMediaPlayer::playbackStateChanged, this, [this]() { Q_EMIT playerStateChanged(); });
}

void DesktopPlayer::loadTrack(const QString &url, const Core::Track &track) {
  QUrl mediaUrl(url);
  if (!mediaUrl.isValid()) {
    qWarning() << "Invalid URL provided to DesktopPlayer:" << url;
    return;
  }

  player->setSource(mediaUrl);
  if (playAfterReady) {
    player->play();
  }
}

void DesktopPlayer::play() {
  if (!player->source().isValid()) {
    playAfterReady = true;
  }
  player->play();
}

void DesktopPlayer::pause() {
  player->pause();
}

Core::PlayerState::State DesktopPlayer::getPlayerState() {
  QMediaPlayer::PlaybackState currentState = player->playbackState();
  switch (currentState) {
  case QMediaPlayer::StoppedState:
    state = PlayerState::Initialized;
    break;
  case QMediaPlayer::PausedState:
    state = PlayerState::Paused;
    break;
  case QMediaPlayer::PlayingState:
    state = PlayerState::Playing;
    break;
  }

  return state;
};

void DesktopPlayer::seekTo(long positionMs) {
  if (player->isSeekable()) {
    player->setPosition(positionMs);
  }
}

long DesktopPlayer::getCurrentPosition() { return player->position(); }

long DesktopPlayer::getDuration() { return player->duration(); }
} // namespace Core
