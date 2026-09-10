#pragma once

#include "Network.h"
#include "YtWorker.h"
#include "Track.h"
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QThread>
#include <qlist.h>
#include <qtmetamacros.h>

class YtManager : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_NAMED_ELEMENT(YT)

public:
  ~YtManager() override;
  static YtManager* instance();
  static YtManager* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

  void requestExtraction(const QString &url);
  void getRelatedTracks(const QString &url);
  Q_INVOKABLE void requestSearch(const QString &query, int maxResults = 5);

Q_SIGNALS:
  void extractionSuccess(const QString &url);
  void extractionFailed(const QString &error);
  void searchSuccess(const QList<Core::Track> &results);
  void searchFailed(const QString &error);
  void getRelatedTracksSuccess(const QList<Core::Track> &tracks);
  void getRelatedTracksFailed(const QString &error);

private:
  // Constructor is private in order to force `QML` Engine to use create()
  // to create the instance
  explicit YtManager(QObject *parent = nullptr);

  QThread *workerThread;
  YtWorker *worker;
  Core::Network::StreamDownloader *downloader;

  Q_SIGNAL void startWorkerInit();
  Q_SIGNAL void startExtraction(const QString &url);
  Q_SIGNAL void startSearch(const QString &query, int maxResults);
  Q_SIGNAL void startGetRelatedTracks(const QString &url);
};
