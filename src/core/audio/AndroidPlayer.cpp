#include "AndroidPlayer.h"
#include <QGuiApplication>
#include <QJniObject>
#include <jni.h>

namespace Core {
AndroidPlayer::AndroidPlayer() {
  QJniObject context = QNativeInterface::QAndroidApplication::context();

  if (context.isValid()) {
    player = QJniObject("Player", "(Landroid/content/Context;)V", context.object());
  } else {
    qWarning() << "Failed to obtain valid Android context for Java Player instantiation.";
  }
  state = PlayerState::Initialized;
}

AndroidPlayer::~AndroidPlayer() {
  if (player.isValid()) {
    player.callMethod<void>("release");
  }
}

void AndroidPlayer::loadTrack(const QString &url, const Core::Track &track) {
  if (!player.isValid()) {
    qDebug() << "AndroidPlayer: Player object isn't valid.";
    return;
  }

  QJniObject jUrl = QJniObject::fromString(url);
  QJniObject jTitle = QJniObject::fromString(track.title);
  QJniObject jArtist = QJniObject::fromString(track.artist);
  QJniObject jArtUrl = QJniObject::fromString(track.thumbnailUrl);

  player.callMethod<void>("loadTrack", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
                          jUrl.object<jstring>(), jTitle.object<jstring>(), jArtist.object<jstring>(),
                          jArtUrl.object<jstring>());

  state = PlayerState::Playing;
}

void AndroidPlayer::play() {
  if (!player.isValid()) {
    qDebug() << "AndroidPlayer: Player object isn't valid.";
    return;
  }

  player.callMethod<void>("play");
  qDebug() << "AndroidPlayer: Playing song now. Setting state.";
  state = PlayerState::Playing;
  qDebug() << "AndroidPlayer: Playing song now. Set state to Playing.";
}

void AndroidPlayer::pause() {
  if (!player.isValid()) {
    qDebug() << "AndroidPlayer: Player object isn't valid.";
    return;
  }

  player.callMethod<void>("pause");
  state = PlayerState::Paused;
}

Core::PlayerState::State AndroidPlayer::getPlayerState() {
  if (!player.isValid()) {
    qDebug() << "AndroidPlayer: Player object isn't valid.";
    return PlayerState::Error;
  }

  return state;
};

void AndroidPlayer::seekTo(long positionMs) {
  if (!player.isValid()) {
    qDebug() << "AndroidPlayer: Player object isn't valid.";
    return;
  }

  jlong jPostitionMs = static_cast<jlong>(positionMs);

  player.callMethod<void>("seekTo", jPostitionMs);
}

long AndroidPlayer::getCurrentPosition() {
  if (!player.isValid()) {
    return 0;
  }
  return player.callMethod<jlong>("getCurrentPosition");
}

long AndroidPlayer::getDuration() {
  if (!player.isValid()) {
    return 0;
  }
  return player.callMethod<jlong>("getDuration");
}
} // namespace Core
