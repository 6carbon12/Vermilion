#include <jni.h>
#include <QMetaObject>
#include <qnamespace.h>
#include <qtpreprocessorsupport.h>
#include "PlayerController.h"

extern "C" {
JNIEXPORT void JNICALL Java_Player_onTrackEnded(JNIEnv *env, jobject thiz) {
    Q_UNUSED(env);
    Q_UNUSED(thiz);
    QMetaObject::invokeMethod(PlayerController::instance(), "next", Qt::DirectConnection);
}
}
