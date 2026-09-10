#include "AndroidPlayer.h"
#include <QGuiApplication>
#include <QJniObject>

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

void AndroidPlayer::play(const QString &url) {
  if (!player.isValid()) {
    qDebug() << "AndroidPlayer: Player object isn't valid.";
    return;
  }

  QJniObject jUrl = QJniObject::fromString(url);

  player.callMethod<void>("play", "(Ljava/lang/String;)V", jUrl.object());
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

void AndroidPlayer::resume() {
  if (!player.isValid()) {
    qDebug() << "AndroidPlayer: Player object isn't valid.";
    return;
  }

  player.callMethod<void>("resume");
  state = PlayerState::Playing;
}

Core::PlayerState::State AndroidPlayer::getPlayerState() {
  if (!player.isValid()) {
    qDebug() << "AndroidPlayer: Player object isn't valid.";
    return PlayerState::Error;
  }

  return state;
};
} // namespace Core
