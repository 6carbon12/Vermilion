#include "YtController.h"
#include "TrackListModel.h"
#include <QQmlEngine>

YtController::YtController(QObject *parent) : QObject(parent) {
  core = new YtEngine(this);

  connect(core, &YtEngine::extractionSuccess, this, &YtController::extractionSuccess, Qt::QueuedConnection);
  connect(core, &YtEngine::extractionFailed, this, &YtController::extractionFailed, Qt::QueuedConnection);
  connect(core, &YtEngine::searchSuccess, this, &YtController::handleSearchResults, Qt::QueuedConnection);
  connect(core, &YtEngine::searchFailed, this, &YtController::searchFailed, Qt::QueuedConnection);
  connect(core, &YtEngine::getRelatedTracksSuccess, this, &YtController::getRelatedTracksSuccess, Qt::QueuedConnection);
  connect(core, &YtEngine::getRelatedTracksFailed, this, &YtController::getRelatedTracksFailed, Qt::QueuedConnection);
}

YtController *YtController::instance() {
  static YtController _instance;
  return &_instance;
}

YtController *YtController::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
  Q_UNUSED(qmlEngine);
  Q_UNUSED(jsEngine);
  YtController *inst = YtController::instance();
  QQmlEngine::setObjectOwnership(inst, QQmlEngine::CppOwnership);
  return inst;
}

void YtController::handleSearchResults(const QList<Core::Track> &results) {
  qDebug() << "handing search";
  Models::TrackListModel *model = new Models::TrackListModel();
  QQmlEngine::setObjectOwnership(model, QQmlEngine::JavaScriptOwnership);
  model->setTracks(results);
  Q_EMIT searchSuccess(model);
}

void YtController::requestExtraction(const QString &url) { core->requestExtraction(url); }
void YtController::requestSearch(const QString &query, int maxResults) { core->requestSearch(query, maxResults); }
void YtController::getRelatedTracks(const QString &url) { core->getRelatedTracks(url); }
