#pragma once

#include "PyHelper.h"
#include "Track.h"
#include <QObject>
#include <pybind11/embed.h>
#include <qtmetamacros.h>

namespace py = pybind11;
class YtWorker : public QObject {
  Q_OBJECT
private:
  /* Modules */
  py::object yt_dlp;
  py::object ytmusicapi;
  /* Objects */
  py::object YoutubeDL;
  py::object YTMusic;
  py::dict getGeneralYdlOpts();

public:
  explicit YtWorker(QObject *parent = nullptr);
  ~YtWorker();
public Q_SLOTS:
  void init();
  void extractUrl(const QString &url);
  void search(const QString &query, int maxResults = 5);
  void getRelatedTracks(const QString &url);
Q_SIGNALS:
  void initFailed(PyHelper::Error error);
  void extractSuccess(const QString &url, const QMap<QByteArray, QByteArray> &headers);
  void extractFailed(const QString &error);
  void searchSuccess(const QList<Core::Track> &results);
  void searchFailed(const QString &error);
  void getRelatedTracksSuccess(const QList<Core::Track> &tracks);
  void getRelatedTracksFailed(const QString &error);
};
