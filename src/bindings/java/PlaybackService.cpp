#include <jni.h>
#include <QMetaObject>
#include <qnamespace.h>
#include <qtpreprocessorsupport.h>
#include "PlayerController.h"

extern "C" {
  JNIEXPORT void JNICALL Java_io_github_x6carbon12_vermilion_PlaybackService_onPlay(JNIEnv *env, jobject thiz) {
    Q_UNUSED(env);
    Q_UNUSED(thiz);
    QMetaObject::invokeMethod(PlayerController::instance(), "play", Qt::DirectConnection);
  }

  JNIEXPORT void JNICALL Java_io_github_x6carbon12_vermilion_PlaybackService_onPause(JNIEnv *env, jobject thiz) {
    Q_UNUSED(env);
    Q_UNUSED(thiz);
    QMetaObject::invokeMethod(PlayerController::instance(), "pause", Qt::DirectConnection);
  }

  JNIEXPORT void JNICALL Java_io_github_x6carbon12_vermilion_PlaybackService_onNext(JNIEnv *env, jobject thiz) {
    Q_UNUSED(env);
    Q_UNUSED(thiz);
    QMetaObject::invokeMethod(PlayerController::instance(), "next", Qt::DirectConnection);
  }

  JNIEXPORT void JNICALL Java_io_github_x6carbon12_vermilion_PlaybackService_onPrev(JNIEnv *env, jobject thiz) {
    Q_UNUSED(env);
    Q_UNUSED(thiz);
    QMetaObject::invokeMethod(PlayerController::instance(), "prev", Qt::DirectConnection);
  }
}
