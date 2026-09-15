#pragma once

#include "Network.h"
#include "Track.h"
#include "YtWorker.h"
#include <QList>
#include <QObject>
#include <QString>
#include <QThread>

class YtEngine : public QObject {
  Q_OBJECT
public:
  explicit YtEngine(QObject *parent = nullptr);
  ~YtEngine();

public Q_SLOTS:
  void requestExtraction(const QString &url);
  void requestSearch(const QString &query, int maxResults);
  void getRelatedTracks(const QString &url);

Q_SIGNALS:
  void extractionSuccess(const QString &url);
  void extractionFailed(const QString &error);
  void searchSuccess(const QList<Core::Track> &results);
  void searchFailed(const QString &error);
  void getRelatedTracksSuccess(const QList<Core::Track> &tracks);
  void getRelatedTracksFailed(const QString &error);

private:
  QThread *workerThread;
  YtWorker *worker;
  Core::Network::StreamDownloader *downloader;

  Q_SIGNAL void startWorkerInit();
  Q_SIGNAL void startExtraction(const QString &url);
  Q_SIGNAL void startSearch(const QString &query, int maxResults);
  Q_SIGNAL void startGetRelatedTracks(const QString &url);
};
