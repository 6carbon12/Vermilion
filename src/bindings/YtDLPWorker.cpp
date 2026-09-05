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
    ydl = py::object();
    YoutubeDL = py::object();
    yt_dlp = py::object();
  }
}

py::dict YtDLPWorker::getYdlOpts() {
  QString nativeLibDir = QCoreApplication::applicationDirPath();
  QString jsBinaryPath = nativeLibDir + "/libqjs.so";

  py::dict runtime_config;
  runtime_config["path"] = jsBinaryPath.toStdString();

  py::dict js_runtimes;
  js_runtimes["quickjs"] = runtime_config;

  py::dict ydl_opts;
  ydl_opts["format"] = "bestaudio/best";
  ydl_opts["quiet"] = true;
  ydl_opts["noplaylist"] = true;
  ydl_opts["js_runtimes"] = js_runtimes;
  ydl_opts["download"] = true;

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

      py::dict ydlOpts = getYdlOpts();
      yt_dlp = py::module_::import("yt_dlp");
      YoutubeDL = yt_dlp.attr("YoutubeDL");
      ydl = YoutubeDL(ydlOpts);
      qCDebug(wrkr) << "Initializtion success.";
    } catch (py::error_already_set &e) {
      qCWarning(wrkr) << "Initializtion failed.";
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
  py::object info = ydl.attr("extract_info")(url.toStdString(), "download"_a = false);

  if (info.contains("url") && !info["url"].is_none()) {
    std::string audioURL = info["url"].cast<std::string>();
    Q_EMIT extractSuccess(QString::fromStdString(audioURL));
    qCDebug(wrkr) << "Extraction success: " << QString::fromStdString(audioURL);
  } else {
    Q_EMIT extractFailed("`url` not found in the info object.");
    qCWarning(wrkr) << "Extraction failed, url was not found in the object.";
  }
}
