#include <jni.h>
#include <QMetaObject>
#include <qtpreprocessorsupport.h>
#include "PlayerManager.h"

extern "C" {
JNIEXPORT void JNICALL Java_Player_onTrackEnded(JNIEnv *env, jobject thiz) {
    Q_UNUSED(env);
    Q_UNUSED(thiz);
    QMetaObject::invokeMethod(PlayerManager::instance(), "next", Qt::QueuedConnection);
}
}
