#pragma once

#include <QQmlAbstractUrlInterceptor>
#include <QString>
#include <QUrl>

/// @brief Simple `QQmlAbstractUrlInterceptor` implementation to intercept all `qrc:/qt/qml/Vermilion/src/qml/...`
/// paths and redirects them to a different folder.
class DebugResourceInterceptor : public QQmlAbstractUrlInterceptor {
public:
  /// @param sourceDir Path to the QML source directory.
  explicit DebugResourceInterceptor(const QString& sourceDir);
  QUrl intercept(const QUrl &path, DataType type) override;

private:
  QString m_sourceDir;
};
