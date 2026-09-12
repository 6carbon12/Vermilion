#include "AndroidPlayer.h"
#include <QGuiApplication>
#include <QJniObject>
#include <jni.h>

namespace Core {
AndroidPlayer::AndroidPlayer() {
  QJniObject context =
      QJniObject::callStaticObjectMethod("org/qtproject/qt/android/QtNative", "activity", "()Landroid/app/Activity;");

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

void AndroidPlayer::setUrl(const QString &url) {
  if (!player.isValid()) {
    qDebug() << "AndroidPlayer: Player object isn't valid.";
    return;
  }

  QJniObject jUrl = QJniObject::fromString(url);
  jstring jUrlStr = jUrl.object<jstring>();
  player.callMethod<void>("setUrl", jUrlStr);
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
