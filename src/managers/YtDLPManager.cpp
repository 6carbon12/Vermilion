#include "YtDLPManager.h"

#include <qlogging.h>

#include <QDebug>
#include <qloggingcategory.h>

Q_LOGGING_CATEGORY(mgr, "YtDLPManager")

YtDLPManager::YtDLPManager(QObject *parent) : QObject(parent)
{
  workerThread = new QThread(this);
  worker = new YtDLPWorker();

  worker->moveToThread(workerThread);

  connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
  connect(this, &YtDLPManager::startWorkerInit, worker, &YtDLPWorker::init, Qt::QueuedConnection);
  connect(this, &YtDLPManager::startExtraction, worker, &YtDLPWorker::extractUrl, Qt::QueuedConnection);

  connect(worker, &YtDLPWorker::initFailed, this, [](PyHelper::Error e) {
    qCFatal(mgr) << "Manager caught YtDLP worker initialization failure:";
  });

  connect(worker, &YtDLPWorker::extractSuccess, this, [this](const QString &url) { Q_EMIT extractionSuccess(url); });

  connect(worker, &YtDLPWorker::extractFailed, this, [this](const QString &err) { Q_EMIT extractionFailed(err); });

  workerThread->start();
  Q_EMIT startWorkerInit();
}

YtDLPManager::~YtDLPManager()
{
  workerThread->quit();
  workerThread->wait();
}

void YtDLPManager::requestExtraction(const QString &url)
{
  qDebug() << "Extraction Requested.";
  Q_EMIT startExtraction(url);
}
