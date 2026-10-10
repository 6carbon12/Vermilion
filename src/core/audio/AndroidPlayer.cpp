#include "AndroidPlayer.h"
#include "Player.h"
#include <QGuiApplication>
#include <QJniObject>
#include <QMetaObject>
#include <jni.h>

namespace Core {
AndroidPlayer::AndroidPlayer() {
  QJniObject context = QNativeInterface::QAndroidApplication::context();
  positionPoolTimer = new QTimer(this);

  positionPoolTimer->setInterval(200);

  if (context.isValid()) {
    player = QJniObject("Player", "(Landroid/content/Context;)V", context.object());
  } else {
    qWarning() << "Failed to obtain valid Android context for Java Player instantiation.";
  }

  connect(positionPoolTimer, &QTimer::timeout, this, [this]() {
    if (state == PlayerState::Playing) {
      Q_EMIT playerPositionChanged();
    }
  });
}

AndroidPlayer::~AndroidPlayer() {
  if (player.isValid()) {
    player.callMethod<void>("release");
  }
}

void AndroidPlayer::play(const QString &url, const Core::Track &track) {
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
  player.callMethod<void>("play");
}

void AndroidPlayer::resume() {
  if (!player.isValid()) {
    qDebug() << "AndroidPlayer: Player object isn't valid.";
    return;
  }

  player.callMethod<void>("play");
}

void AndroidPlayer::pause() {
  if (!player.isValid()) {
    qDebug() << "AndroidPlayer: Player object isn't valid.";
    return;
  }

  player.callMethod<void>("pause");
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

void AndroidPlayer::emitRequestNext() { Q_EMIT requestNext(); }

void AndroidPlayer::emitRequestPrev() { Q_EMIT requestPrev(); }

void AndroidPlayer::handlePlayerStateChanged() {
  jint playerState = player.callMethod<jint>("getPlayerState");
  state = getPlayerStateFromInt(playerState);

  if (state == PlayerState::Playing) {
    if (!positionPoolTimer->isActive())
      positionPoolTimer->start();
  } else {
    if (positionPoolTimer->isActive())
      positionPoolTimer->stop();
  }

  Q_EMIT playerStateChanged();
}

Core::PlayerState::State AndroidPlayer::getPlayerStateFromInt(int playerState) {
  switch (playerState) {
  case 0:
    return PlayerState::Initialized;
  case 1:
    return PlayerState::Playing;
  case 2:
    return PlayerState::Paused;
  default:
    return PlayerState::Error;
  }
}
} // namespace Core

extern "C" {
JNIEXPORT void JNICALL Java_io_github_x6carbon12_vermilion_PlaybackService_requestNext(JNIEnv *env, jobject thiz) {
  Q_UNUSED(env);
  Q_UNUSED(thiz);
  Core::AndroidPlayer *androidPlayerPtr = static_cast<Core::AndroidPlayer *>(Core::Player::instance());
  QMetaObject::invokeMethod(androidPlayerPtr, "emitRequestNext", Qt::DirectConnection);
}

JNIEXPORT void JNICALL Java_io_github_x6carbon12_vermilion_PlaybackService_requestPrev(JNIEnv *env, jobject thiz) {
  Q_UNUSED(env);
  Q_UNUSED(thiz);
  Core::AndroidPlayer *androidPlayerPtr = static_cast<Core::AndroidPlayer *>(Core::Player::instance());
  QMetaObject::invokeMethod(androidPlayerPtr, "emitRequestPrev", Qt::DirectConnection);
}

JNIEXPORT void JNICALL Java_io_github_x6carbon12_vermilion_PlaybackService_playerStateChanged(JNIEnv *env,
                                                                                              jobject thiz) {
  Q_UNUSED(env);
  Q_UNUSED(thiz);
  Core::AndroidPlayer *androidPlayerPtr = static_cast<Core::AndroidPlayer *>(Core::Player::instance());
  // Must be QueuedConnection since timers can't be started from another thread.
  QMetaObject::invokeMethod(androidPlayerPtr, "handlePlayerStateChanged", Qt::QueuedConnection);
}
}
