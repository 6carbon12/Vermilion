#include "YtDLPWorker.h"
#include <QCoreApplication>
#include <QStandardPaths>
#include <QString>
#include <pybind11/embed.h>
#include <qloggingcategory.h>
#include <qtmetamacros.h>

namespace py = pybind11;

Q_LOGGING_CATEGORY(wrkr, "YtDLPWorker")

YtDLPWorker::YtDLPWorker(QObject *parent) : QObject(parent) {}

YtDLPWorker::~YtDLPWorker() {
  if (Py_IsInitialized()) {
    py::gil_scoped_acquire acquire;
    YoutubeDL = py::object();
    yt_dlp = py::object();
  }
}

py::dict YtDLPWorker::getGeneralYdlOpts() {
  QString nativeLibDir = QCoreApplication::applicationDirPath();
  QString jsBinaryPath = nativeLibDir + "/libqjs.so";

  py::dict runtime_config;
  runtime_config["path"] = jsBinaryPath.toStdString();

  py::dict js_runtimes;
  js_runtimes["quickjs"] = runtime_config;

  py::list client_args;
  client_args.append("android");
  client_args.append("ios");
  client_args.append("web");

  py::dict yt_args;
  yt_args["player_client"] = client_args;

  py::dict extractor_args;
  extractor_args["youtube"] = yt_args;

  py::dict ydl_opts;
  ydl_opts["format"] = "bestaudio/best";
  ydl_opts["quiet"] = true;
  ydl_opts["no_warnings"] = true;
  ydl_opts["noplaylist"] = true;
  ydl_opts["skip_download"] = true;
  ydl_opts["extractor_args"] = extractor_args;
  ydl_opts["js_runtimes"] = js_runtimes;

  return ydl_opts;
}

void YtDLPWorker::init() {
  const std::array<QString, 3> modules = {"assets:/yt_dlp", "assets:/yt_dlp_ejs", "assets:/certifi"};

  for (const auto &modulePath : modules) {
    if (auto result = PyHelper::installModule(modulePath); !result) {
      PyHelper::Error e = result.error();
      Q_EMIT initFailed(e);
      qCWarning(wrkr) << "Initializtion failed.";
      qCWarning(wrkr) << e.debugContext;
      return;
    }
  }

  {
    py::gil_scoped_acquire acquire;

    try {
      QString nativeLibDir = QCoreApplication::applicationDirPath();
      py::module_ sys = py::module_::import("sys");
      sys.attr("path").attr("append")(nativeLibDir.toStdString());

      // Invalidate caches to force a rescan of modules.
      py::module_::import("importlib").attr("invalidate_caches")();

      py::dict ydlOpts = getGeneralYdlOpts();
      yt_dlp = py::module_::import("yt_dlp");
      YoutubeDL = yt_dlp.attr("YoutubeDL");
      qCDebug(wrkr) << "Initializtion success.";
    } catch (py::error_already_set &e) {
      qCWarning(wrkr) << "Initializtion failed.";
      qCWarning(wrkr) << e.what();
      Q_EMIT initFailed(PyHelper::Error({PyHelper::ErrorReason::ImportFailed, e.what()}));
    }
  }
}

void YtDLPWorker::extractUrl(const QString &url) {
  if (!ydl || ydl.is_none()) {
    Q_EMIT extractFailed("yt-dlp is not initialized. Cannot extract URL.");
    qCWarning(wrkr) << "yt-dlp is not initialized. Cannot extract URL.";
    return;
  }
  using namespace pybind11::literals;
  py::gil_scoped_acquire acquire;

  qCDebug(wrkr) << "Extracting info";
  py::dict ydlOpts = getGeneralYdlOpts();
  py::object ydl = YoutubeDL(ydlOpts);
  py::object info = ydl.attr("extract_info")(url.toStdString(), "download"_a = false);

  if (info.contains("url") && !info["url"].is_none()) {
    QString audioURL = QString::fromStdString(info["url"].cast<std::string>());
    qCDebug(wrkr) << "Audio URL extraction success:" << audioURL;

    QMap<QByteArray, QByteArray> headersMap;
    if (info.contains("http_headers")) {
      py::dict headers = info["http_headers"].cast<py::dict>();
      for (auto item : headers) {
        QByteArray key = QByteArray::fromStdString(item.first.cast<std::string>());
        QByteArray value = QByteArray::fromStdString(item.second.cast<std::string>());
        headersMap.insert(key, value);
      }
      qCDebug(wrkr) << "Headers extraction success:" << headersMap;
    }

    Q_EMIT extractSuccess(audioURL, headersMap);
    qCDebug(wrkr) << "Extraction completed.";
  } else {
    Q_EMIT extractFailed("`url` not found in the info object.");
    qCWarning(wrkr) << "Extraction failed, url was not found in the object.";
  }
}
