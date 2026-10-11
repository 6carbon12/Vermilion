#include <HotReloader.h>
#include <QTimer>

HotReloader::HotReloader(QQmlApplicationEngine *engine, QString qmlPath, QObject *parent)
    : QObject(parent), m_engine(engine), m_qmlPath(qmlPath) {
#ifdef QT_DEBUG
  m_watcher.addPath(qmlPath);
  m_watcher.addPath(qmlPath + "/pages");
  m_watcher.addPath(qmlPath + "/widgets");
  m_watcher.addPath(qmlPath + "/components");
  connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [this]() {
    Q_EMIT fileChanged();
  });
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
        m_engine->clearComponentCache();
        Q_EMIT reloadUI();
      },
      Qt::QueuedConnection);
#endif
}
