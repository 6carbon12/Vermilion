#pragma once

#include "../bindings/YtDLPWorker.h"
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QThread>
#include <qtmetamacros.h>
#include "Network.h"

class YtDLPManager : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_NAMED_ELEMENT(YtDLP)

public:
  explicit YtDLPManager(QObject *parent = nullptr);
  ~YtDLPManager() override;

  Q_INVOKABLE void requestExtraction(const QString &url);
  Q_INVOKABLE void requestSearch(const QString &query, int maxResults = 5);

Q_SIGNALS:
  void extractionSuccess(const QString &url);
  void extractionFailed(const QString &error);
  void searchSuccess(const QVariantList &results);
  void searchFailed(const QString &error);

private:
  QThread *workerThread;
  YtDLPWorker *worker;
  Core::Network::StreamDownloader *downloader;

  Q_SIGNAL void startWorkerInit();
  Q_SIGNAL void startExtraction(const QString &url);
  Q_SIGNAL void startSearch(const QString &query, int maxResults);
};
