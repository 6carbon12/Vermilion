#include "YtDLPWorker.h"
#include <QCoreApplication>
#include <QDir>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
#include <pybind11/embed.h>
#include <qtmetamacros.h>

namespace py = pybind11;

Q_LOGGING_CATEGORY(YtDLPWorker_l, "YtDLPWorker")

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
  QString yt_dlpCacheDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/yt-dlp";
  QDir().mkpath(yt_dlpCacheDir);

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
  ydl_opts["cachedir"] = yt_dlpCacheDir.toStdString();

  return ydl_opts;
}

void YtDLPWorker::init() {
  const std::array<QString, 9> modules = {
      "assets:/yt_dlp",  "assets:/yt_dlp_ejs",         "assets:/certifi", "assets:/ytmusicapi", "assets:/requests",
      "assets:/urllib3", "assets:/charset_normalizer", "assets:/idna",    "assets:/setuptools"};

  for (const auto &modulePath : modules) {
    if (auto result = PyHelper::installModule(modulePath); !result) {
      PyHelper::Error e = result.error();
      Q_EMIT initFailed(e);
      qCWarning(YtDLPWorker_l) << "Initializtion failed.";
      qCWarning(YtDLPWorker_l) << "Failed to install module: " << modulePath;
      qCWarning(YtDLPWorker_l) << e.debugContext;
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
      yt_dlp = py::module_::import("ytmusicapi");
      YTMusic = yt_dlp.attr("YTMusic")();
      qCDebug(YtDLPWorker_l) << "Initializtion success.";
    } catch (py::error_already_set &e) {
      qCWarning(YtDLPWorker_l) << "Initializtion failed.";
      qCWarning(YtDLPWorker_l) << e.what();
      Q_EMIT initFailed(PyHelper::Error({PyHelper::ErrorReason::ImportFailed, e.what()}));
    }
  }
}

void YtDLPWorker::search(const QString &query, int maxResults) {
  qCDebug(YtDLPWorker_l) << "Searching query: " << query;
  using namespace pybind11::literals;
  py::gil_scoped_acquire acquire;

  QVariantList resultsList;
  try {
    py::object results = YTMusic.attr("search")(query.toStdString(), "filter"_a = "songs", "limit"_a = maxResults);

    for (py::handle result : results) {
      py::dict resultDict = result.cast<py::dict>();

      auto getStrMember = [&](const char *member) -> QString {
        if (resultDict.contains(member) && !resultDict[member].is_none()) {
          return QString::fromStdString(resultDict[member].cast<std::string>());
        } else {
          qCWarning(YtDLPWorker_l) << "Unable to get member: " << member << ". From a result of ytmusicapi";
          return "N/A";
        }
      };

      QVariantMap resultData;
      resultData["title"] = getStrMember("title");
      resultData["url"] = "https://youtube.com/watch?v=" + getStrMember("videoId");
      resultData["viewCount"] = getStrMember("views");
      resultData["duration"] = getStrMember("duration");

      if (resultDict.contains("thumbnails") && !resultDict["thumbnails"].is_none()) {
        py::list thumbs = resultDict["thumbnails"].cast<py::list>();
        if (!thumbs.empty()) {
          py::dict bestThumb = thumbs[thumbs.size() - 1].cast<py::dict>();
          std::string thumbUrl = bestThumb["url"].cast<std::string>();
          resultData["thumbnail"] = QString::fromStdString(thumbUrl);
        } else {
          qCWarning(YtDLPWorker_l) << "Thumbnails empty";
        }
      } else {
        qCWarning(YtDLPWorker_l) << "Unable to get member: " << "thumbnails" << ". From a result of ytmusicapi";
      }

      resultsList.append(resultData);
    }

    Q_EMIT searchSuccess(resultsList);
  } catch (const py::error_already_set &e) {
    qCDebug(YtDLPWorker_l) << "Search failed" << e.what();
    Q_EMIT searchFailed(QString::fromStdString(e.what()));
  } catch (const std::exception &e) {
    qCDebug(YtDLPWorker_l) << "Search failed" << e.what();
    Q_EMIT searchFailed(QString::fromStdString(e.what()));
  }
}

void YtDLPWorker::extractUrl(const QString &url) {
  using namespace pybind11::literals;
  py::gil_scoped_acquire acquire;

  qCDebug(YtDLPWorker_l) << "Extracting info";
  py::dict ydlOpts = getGeneralYdlOpts();
  py::object ydl = YoutubeDL(ydlOpts);
  py::object info = ydl.attr("extract_info")(url.toStdString(), "download"_a = false);

  if (info.contains("url") && !info["url"].is_none()) {
    QString audioURL = QString::fromStdString(info["url"].cast<std::string>());
    qCDebug(YtDLPWorker_l) << "Audio URL extraction success:" << audioURL;

    QMap<QByteArray, QByteArray> headersMap;
    if (info.contains("http_headers")) {
      py::dict headers = info["http_headers"].cast<py::dict>();
      for (auto item : headers) {
        QByteArray key = QByteArray::fromStdString(item.first.cast<std::string>());
        QByteArray value = QByteArray::fromStdString(item.second.cast<std::string>());
        headersMap.insert(key, value);
      }
      qCDebug(YtDLPWorker_l) << "Headers extraction success:" << headersMap;
    }

    Q_EMIT extractSuccess(audioURL, headersMap);
    qCDebug(YtDLPWorker_l) << "Extraction completed.";
  } else {
    Q_EMIT extractFailed("`url` not found in the info object.");
    qCWarning(YtDLPWorker_l) << "Extraction failed, url was not found in the object.";
  }
}
