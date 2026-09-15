#pragma once

#include "YtEngine.h"
#include "Track.h"
#include <QObject>
#include <QQmlEngine>
#include <QString>

class YtController : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_NAMED_ELEMENT(YT)

public:
  ~YtController() override = default;
  static YtController* instance();
  static YtController* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

  void requestExtraction(const QString &url);
  void getRelatedTracks(const QString &url);
  Q_INVOKABLE void requestSearch(const QString &query, int maxResults = 5);

  YtEngine* getCore() const { return core; }

Q_SIGNALS:
  void extractionSuccess(const QString &url);
  void extractionFailed(const QString &error);
  void searchSuccess(const QList<Core::Track> &results);
  void searchFailed(const QString &error);
  void getRelatedTracksSuccess(const QList<Core::Track> &tracks);
  void getRelatedTracksFailed(const QString &error);

private:
  explicit YtController(QObject *parent = nullptr);
  YtEngine *core;
};
