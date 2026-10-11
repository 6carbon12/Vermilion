#pragma once

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
  QML_SINGLETON
  QML_ELEMENT
  Q_PROPERTY(QVariantList savedStackHistory READ getSavedStackHistory WRITE setSavedStackHistory NOTIFY
                 savedStackHistoryChanged)

public:
  HotReloader(QObject *parent = nullptr);

  QVariantList getSavedStackHistory() const;
  void setSavedStackHistory(const QVariantList &list);
  Q_INVOKABLE void requestReload();

Q_SIGNALS:
  // @brief Emitted when any files have been changed.
  void fileChanged();
  // @brief Emitted when UI needs to be reloaded.
  void reloadUI();
  void savedStackHistoryChanged();

private:
  QVariantList m_savedStackHistory;
#ifdef QT_DEBUG
  QFileSystemWatcher m_watcher;
#endif
};
