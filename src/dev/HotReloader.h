#include <qobject.h>
#include <qqmlapplicationengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#ifdef QT_DEBUG
#include <QFileSystemWatcher>
#endif

// @brief This is a bridge class between `ApplicationWindow` and the Contents.
//
// Contents shall react to `fileChanged` and save its stack history into `HotReloader` and request a reload.
// `ApplicationWindow` shall react to `reloadUI` and reload the Contents.
class HotReloader : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("HotReloader is initalized by CPP.")
  Q_PROPERTY(QVariantList savedStackHistory READ getSavedStackHistory WRITE setSavedStackHistory NOTIFY savedStackHistoryChanged)

public:
  HotReloader(QQmlApplicationEngine* engine, QString qmlPath, QObject *parent = nullptr);
  QVariantList getSavedStackHistory() const;
  void setSavedStackHistory(const QVariantList& list);
  Q_INVOKABLE void requestReload();

Q_SIGNALS:
  // @brief Emitted when any files have been changed.
  void fileChanged();
  // @brief Emitted when UI needs to be reloaded.
  void reloadUI();
  void savedStackHistoryChanged();

private:
  QVariantList m_savedStackHistory;
  QQmlApplicationEngine *m_engine;
  QString m_qmlPath;
#ifdef QT_DEBUG
  QFileSystemWatcher m_watcher;
#endif
};
