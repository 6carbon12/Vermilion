#include "DebugResourceInterceptor.h"
#include <QFile>

DebugResourceInterceptor::DebugResourceInterceptor(const QString &sourceDir) : m_sourceDir(sourceDir) {}

QUrl DebugResourceInterceptor::intercept(const QUrl &path, DataType _) {
  if (path.scheme() == "qrc") {
    QString qrcPath = path.path();

    if (qrcPath.startsWith("/qt/qml/Vermilion/src/qml")) {
      QString localPath = qrcPath;
      localPath.replace("/qt/qml/Vermilion/src/qml", m_sourceDir);

      if (QFile::exists(localPath)) {
        return QUrl::fromLocalFile(localPath);
      }
    }
  }
  return path;
}
