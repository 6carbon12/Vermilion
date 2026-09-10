#include "YtManager.h"
#include <QDebug>
#include <QLoggingCategory>
#include <QtLogging>

Q_LOGGING_CATEGORY(mgr, "YtManager")

YtManager::YtManager(QObject *parent)
    : QObject(parent), workerThread(nullptr), worker(nullptr), downloader(nullptr) {
  using Core::Network::StreamDownloader;

  workerThread = new QThread(this);
  worker = new YtWorker();
  downloader = new Core::Network::StreamDownloader();

  worker->moveToThread(workerThread);

  connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
  connect(workerThread, &QThread::finished, downloader, &QObject::deleteLater);

  connect(this, &YtManager::startWorkerInit, worker, &YtWorker::init, Qt::QueuedConnection);
  connect(this, &YtManager::startExtraction, worker, &YtWorker::extractUrl, Qt::QueuedConnection);
  connect(this, &YtManager::startSearch, worker, &YtWorker::search, Qt::QueuedConnection);

  connect(worker, &YtWorker::initFailed, this, [](PyHelper::Error e) {
    qCFatal(mgr) << "Failed to initalize worker... App might not work." << e.debugContext;
  });
  connect(worker, &YtWorker::extractSuccess, this,
          [this](const QString &url, const QMap<QByteArray, QByteArray> &headersMap) {
            downloader->startDownload(url, headersMap);
          });
  connect(worker, &YtWorker::extractFailed, this, [this](const QString &err) { Q_EMIT extractionFailed(err); });
  connect(worker, &YtWorker::searchSuccess, this,
          [this](const QList<Core::Track> &results) { Q_EMIT searchSuccess(results); });
  connect(worker, &YtWorker::searchFailed, this, [this](const QString &err) { Q_EMIT searchFailed(err); });

  connect(downloader, &StreamDownloader::bufferReady, this,
          [this](const QString &localFileUrl) { Q_EMIT extractionSuccess(localFileUrl); });
  connect(downloader, &StreamDownloader::downloadFailed, this,
          [this](const QString &err) { Q_EMIT extractionFailed(err); });

  workerThread->start();
  Q_EMIT startWorkerInit();
}

YtManager::~YtManager() {
  workerThread->quit();
  workerThread->wait();
}

YtManager* YtManager::instance() {
  static YtManager _instance;
  return &_instance;
}

YtManager* YtManager::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
  Q_UNUSED(qmlEngine);
  Q_UNUSED(jsEngine);
  return YtManager::instance();
}

void YtManager::requestExtraction(const QString &url) { Q_EMIT startExtraction(url); }

void YtManager::requestSearch(const QString &query, int maxResults) { Q_EMIT startSearch(query, maxResults); }
