#pragma once

#include "PyHelper.h"
#include <QObject>
#include <pybind11/embed.h>
#include <qtmetamacros.h>

namespace py = pybind11;
class YtDLPWorker : public QObject {
  Q_OBJECT
private:
  py::object yt_dlp;
  py::object YoutubeDL;
  py::dict getGeneralYdlOpts();

public:
  explicit YtDLPWorker(QObject *parent = nullptr);
  ~YtDLPWorker();
public Q_SLOTS:
  void init();
  void extractUrl(const QString &url);
Q_SIGNALS:
  void initFailed(PyHelper::Error e);
  void extractSuccess(const QString &url, const QMap<QByteArray, QByteArray> &headers);
  void extractFailed(const QString &e);
};
