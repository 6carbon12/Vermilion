#include "YtEngine.h"
#include <QCoreApplication>
#include <QDebug>
#include <QThread>

YtEngine::YtEngine(QObject *parent)
    : QObject(nullptr), workerThread(nullptr), worker(nullptr), downloader(nullptr) {
  using Core::Network::StreamDownloader;

  workerThread = new QThread(this);
  worker = new YtWorker();
  downloader = new Core::Network::StreamDownloader();

  this->moveToThread(workerThread);
  worker->moveToThread(workerThread);
  downloader->moveToThread(workerThread);

  connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
  connect(workerThread, &QThread::finished, downloader, &QObject::deleteLater);

  connect(this, &YtEngine::startWorkerInit, worker, &YtWorker::init, Qt::QueuedConnection);
  connect(this, &YtEngine::startExtraction, worker, &YtWorker::extractUrl, Qt::QueuedConnection);
  connect(this, &YtEngine::startSearch, worker, &YtWorker::search, Qt::QueuedConnection);
  connect(this, &YtEngine::startGetRelatedTracks, worker, &YtWorker::getRelatedTracks, Qt::QueuedConnection);

  connect(worker, &YtWorker::extractSuccess, downloader, &Core::Network::StreamDownloader::startDownload,
          Qt::QueuedConnection);

  connect(
      worker, &YtWorker::extractFailed, this, [this](const QString &error) { Q_EMIT extractionFailed(error); },
      Qt::QueuedConnection);

  connect(
      worker, &YtWorker::searchSuccess, this,
      [this](const QList<Core::Track> &response) { Q_EMIT searchSuccess(response); }, Qt::QueuedConnection);

  connect(
      worker, &YtWorker::searchFailed, this, [this](const QString &error) { Q_EMIT searchFailed(error); },
      Qt::QueuedConnection);

  connect(
      worker, &YtWorker::getRelatedTracksFailed, this,
      [this](const QString &error) { Q_EMIT getRelatedTracksFailed(error); }, Qt::QueuedConnection);

  connect(
      worker, &YtWorker::getRelatedTracksSuccess, this,
      [this](const QList<Core::Track> &track) { Q_EMIT getRelatedTracksSuccess(track); }, Qt::QueuedConnection);

  connect(
      downloader, &StreamDownloader::bufferReady, this,
      [this](const QString &localFileUrl) { Q_EMIT extractionSuccess(localFileUrl); }, Qt::QueuedConnection);

  connect(
      downloader, &StreamDownloader::downloadFailed, this,
      [this](const QString &error) { Q_EMIT extractionFailed(error); }, Qt::QueuedConnection);

  workerThread->start();
  Q_EMIT startWorkerInit();
}

YtEngine::~YtEngine() {
  workerThread->quit();
  workerThread->wait();
}

void YtEngine::requestExtraction(const QString &url) { Q_EMIT startExtraction(url); }

void YtEngine::requestSearch(const QString &query, int maxResults) { Q_EMIT startSearch(query, maxResults); }

void YtEngine::getRelatedTracks(const QString &url) { Q_EMIT startGetRelatedTracks(url); }
