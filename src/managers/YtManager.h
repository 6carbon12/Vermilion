#pragma once

#include "Network.h"
#include "YtWorker.h"
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QThread>
#include <qtmetamacros.h>

class YtManager : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_NAMED_ELEMENT(YT)

public:
  explicit YtManager(QObject *parent = nullptr);
  ~YtManager() override;

  Q_INVOKABLE void requestExtraction(const QString &url);
  Q_INVOKABLE void requestSearch(const QString &query, int maxResults = 5);

Q_SIGNALS:
  void extractionSuccess(const QString &url);
  void extractionFailed(const QString &error);
  void searchSuccess(const QVariantList &results);
  void searchFailed(const QString &error);

private:
  QThread *workerThread;
  YtWorker *worker;
  Core::Network::StreamDownloader *downloader;

  Q_SIGNAL void startWorkerInit();
  Q_SIGNAL void startExtraction(const QString &url);
  Q_SIGNAL void startSearch(const QString &query, int maxResults);
};
