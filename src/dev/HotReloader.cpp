#include <HotReloader.h>
#include <QQmlEngine>

HotReloader::HotReloader(QObject *parent) : QObject(parent) {
#ifdef QT_DEBUG
#define XSTR(x) STR(x)
#define STR(x) #x
  QString qmlPath = QString::fromUtf8(XSTR(HOTRELOAD_QML_DIR)).remove('"');

  m_watcher.addPath(qmlPath);
  m_watcher.addPath(qmlPath + "/pages");
  m_watcher.addPath(qmlPath + "/widgets");
  m_watcher.addPath(qmlPath + "/components");

  connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [this]() { Q_EMIT fileChanged(); });
  qDebug() << "Initalized HotReloader.";
#endif
}

void HotReloader::setSavedStackHistory(const QVariantList &list) {
  m_savedStackHistory = list;
  Q_EMIT savedStackHistoryChanged();
}

QVariantList HotReloader::getSavedStackHistory() const { return m_savedStackHistory; }

void HotReloader::requestReload() {
#ifdef QT_DEBUG
  QMetaObject::invokeMethod(
      this,
      [this]() {
        if (auto engine = qmlEngine(this)) {
          engine->clearComponentCache();
        }
        Q_EMIT reloadUI();
      },
      Qt::QueuedConnection);
#endif
}
