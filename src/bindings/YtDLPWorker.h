#pragma once

#include <QObject>
#include <qtmetamacros.h>
#include "PyHelper.h"
#include <pybind11/embed.h>

namespace py = pybind11;
class YtDLPWorker : public QObject
{
  Q_OBJECT
  private:
    py::object yt_dlp;
    py::object YoutubeDL;
    py::object ydl;
    py::dict getYdlOpts();
  public:
    explicit YtDLPWorker(QObject *parent = nullptr);
    ~YtDLPWorker();
  public Q_SLOTS:
    void init();
    void extractUrl(const QString& url);
  Q_SIGNALS:
    void initFailed(PyHelper::Error e);
    void extractSuccess(const QString& url);
    void extractFailed(const QString& e);
};
