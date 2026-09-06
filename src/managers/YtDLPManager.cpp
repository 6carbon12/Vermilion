#include "YtDLPManager.h"
#include <QDebug>
#include <cstdlib>
#include <qlogging.h>
#include <qloggingcategory.h>
#include <unistd.h>

Q_LOGGING_CATEGORY(mgr, "YtDLPManager")

YtDLPManager::YtDLPManager(QObject *parent)
    : QObject(parent), workerThread(nullptr), worker(nullptr), downloader(nullptr) {
  using Core::Network::StreamDownloader;

  workerThread = new QThread(this);
  worker = new YtDLPWorker();
  downloader = new Core::Network::StreamDownloader();

  worker->moveToThread(workerThread);

  connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
  connect(workerThread, &QThread::finished, downloader, &QObject::deleteLater);
  connect(this, &YtDLPManager::startWorkerInit, worker, &YtDLPWorker::init, Qt::QueuedConnection);
  connect(this, &YtDLPManager::startExtraction, worker, &YtDLPWorker::extractUrl, Qt::QueuedConnection);

  connect(worker, &YtDLPWorker::initFailed, this, [](PyHelper::Error e) {
    qCFatal(mgr) << "Failed to initalize worker... App might not work." << e.debugContext;
  });

  connect(worker, &YtDLPWorker::extractSuccess, this,
          [this](const QString &url, const QMap<QByteArray, QByteArray> &headersMap) {
            downloader->startDownload(url, headersMap);
          });

  connect(downloader, &StreamDownloader::bufferReady, this,
          [this](const QString &localFileUrl) { Q_EMIT extractionSuccess(localFileUrl); });

  connect(downloader, &StreamDownloader::downloadFailed, this,
          [this](const QString &err) { Q_EMIT extractionFailed(err); });

  connect(worker, &YtDLPWorker::extractFailed, this, [this](const QString &err) { Q_EMIT extractionFailed(err); });

  workerThread->start();
  Q_EMIT startWorkerInit();
}

YtDLPManager::~YtDLPManager() {
  workerThread->quit();
  workerThread->wait();
}

void YtDLPManager::requestExtraction(const QString &url) { Q_EMIT startExtraction(url); }
