#include "YtWorker.h"
#include <QCoreApplication>
#include <QDir>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
#include <pybind11/embed.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>

namespace py = pybind11;

Q_LOGGING_CATEGORY(YtWorker_l, "YtWorker")

YtWorker::YtWorker(QObject *parent) : QObject(parent) {}

YtWorker::~YtWorker() {
  if (Py_IsInitialized()) {
    py::gil_scoped_acquire acquire;
    YoutubeDL = py::object();
    yt_dlp = py::object();
    YTMusic = py::object();
    ytmusicapi = py::object();
  }
}

py::dict YtWorker::getGeneralYdlOpts() {
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

void YtWorker::init() {
  const std::array<QString, 8> modules = {
      "assets:/yt_dlp",  "assets:/yt_dlp_ejs",         "assets:/certifi", "assets:/ytmusicapi", "assets:/requests",
      "assets:/urllib3", "assets:/charset_normalizer", "assets:/idna"};

  for (const auto &modulePath : modules) {
    if (auto result = PyHelper::installModule(modulePath); !result) {
      PyHelper::Error e = result.error();
      Q_EMIT initFailed(e);
      qCWarning(YtWorker_l) << "Initializtion failed.";
      qCWarning(YtWorker_l) << "Failed to install module: " << modulePath;
      qCWarning(YtWorker_l) << e.debugContext;
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
      qCDebug(YtWorker_l) << "Initializtion success.";
    } catch (py::error_already_set &e) {
      qCWarning(YtWorker_l) << "Initializtion failed.";
      qCWarning(YtWorker_l) << e.what();
      Q_EMIT initFailed(PyHelper::Error({PyHelper::ErrorReason::ImportFailed, e.what()}));
    }
  }
}

void YtWorker::search(const QString &query, int maxResults) {
  qCDebug(YtWorker_l) << "Searching query: " << query;
  using namespace pybind11::literals;
  py::gil_scoped_acquire acquire;

  QList<Core::Track> resultsList;
  try {
    py::object results = YTMusic.attr("search")(query.toStdString(), "filter"_a = "songs", "limit"_a = maxResults);

    for (py::handle result : results) {
      py::dict resultDict = result.cast<py::dict>();

      auto getStrMember = [&](const char *member) -> QString {
        if (resultDict.contains(member) && !resultDict[member].is_none()) {
          return QString::fromStdString(resultDict[member].cast<std::string>());
        } else {
          qCWarning(YtWorker_l) << "Unable to get member: " << member << ". From a result of ytmusicapi";
          return "N/A";
        }
      };

      Core::Track resultData;
      resultData.title = getStrMember("title");
      resultData.url = "https://youtube.com/watch?v=" + getStrMember("videoId");
      resultData.views = getStrMember("views");
      resultData.duration = getStrMember("duration");

      if (resultDict.contains("thumbnails") && !resultDict["thumbnails"].is_none()) {
        py::list thumbs = resultDict["thumbnails"].cast<py::list>();
        if (!thumbs.empty()) {
          py::dict bestThumb = thumbs[thumbs.size() - 1].cast<py::dict>();
          std::string thumbUrl = bestThumb["url"].cast<std::string>();
          resultData.thumbnailUrl = QString::fromStdString(thumbUrl);
        } else {
          qCWarning(YtWorker_l) << "Thumbnails empty";
        }
      } else {
        qCWarning(YtWorker_l) << "Unable to get member: " << "thumbnails" << ". From a result of ytmusicapi";
      }

      resultsList.append(resultData);
    }

    Q_EMIT searchSuccess(resultsList);
  } catch (const py::error_already_set &e) {
    qCDebug(YtWorker_l) << "Search failed" << e.what();
    Q_EMIT searchFailed(QString::fromStdString(e.what()));
  } catch (const std::exception &e) {
    qCDebug(YtWorker_l) << "Search failed" << e.what();
    Q_EMIT searchFailed(QString::fromStdString(e.what()));
  }
}

void YtWorker::extractUrl(const QString &url) {
  using namespace pybind11::literals;
  py::gil_scoped_acquire acquire;

  qCDebug(YtWorker_l) << "Extracting info";
  py::dict ydlOpts = getGeneralYdlOpts();
  py::object ydl = YoutubeDL(ydlOpts);
  py::object info = ydl.attr("extract_info")(url.toStdString(), "download"_a = false);

  if (info.contains("url") && !info["url"].is_none()) {
    QString audioURL = QString::fromStdString(info["url"].cast<std::string>());
    qCDebug(YtWorker_l) << "Audio URL extraction success:" << audioURL;

    QMap<QByteArray, QByteArray> headersMap;
    if (info.contains("http_headers")) {
      py::dict headers = info["http_headers"].cast<py::dict>();
      for (auto item : headers) {
        QByteArray key = QByteArray::fromStdString(item.first.cast<std::string>());
        QByteArray value = QByteArray::fromStdString(item.second.cast<std::string>());
        headersMap.insert(key, value);
      }
      qCDebug(YtWorker_l) << "Headers extraction success:" << headersMap;
    }

    Q_EMIT extractSuccess(audioURL, headersMap);
    qCDebug(YtWorker_l) << "Extraction completed.";
  } else {
    Q_EMIT extractFailed("`url` not found in the info object.");
    qCWarning(YtWorker_l) << "Extraction failed, url was not found in the object.";
  }
}

void YtWorker::getRelatedTracks(const QString &url) {
  using namespace pybind11::literals;
  py::gil_scoped_acquire acquire;
  QList<Core::Track> relatedTracks;
  QRegularExpression regex("v=([a-zA-Z0-9_-]{11})");

  QRegularExpressionMatch match = regex.match(url);
  QString videoId{};
  if (match.hasMatch()) {
    videoId = match.captured(1);
    if (videoId.length() != 11) {
      Q_EMIT getRelatedTracksFailed("Invalid Regex match.");
      return;
    }
  } else {
    Q_EMIT getRelatedTracksFailed("Invalid URL.");
    return;
  }

  std::string videoIdStd = videoId.toStdString();
  try {
    py::object output = YTMusic.attr("get_watch_playlist")("videoId"_a = videoIdStd, "radio"_a = true);
    py::list tracks = output["tracks"].cast<py::list>();

    for (py::handle track : tracks) {
      py::dict trackDict = track.cast<py::dict>();

      auto getStrMember = [&](const char *member) -> QString {
        if (trackDict.contains(member) && !trackDict[member].is_none()) {
          return QString::fromStdString(trackDict[member].cast<std::string>());
        } else {
          qCWarning(YtWorker_l) << "Unable to get member: " << member << ". From a result of ytmusicapi";
          return "N/A";
        }
      };

      Core::Track resultData;
      resultData.title = getStrMember("title");
      qCDebug(YtWorker_l) << "Got reated title: " << resultData.title;
      resultData.url = "https://youtube.com/watch?v=" + getStrMember("videoId");
      resultData.views = getStrMember("views");
      resultData.duration = getStrMember("length");

      if (trackDict.contains("thumbnails") && !trackDict["thumbnails"].is_none()) {
        py::list thumbs = trackDict["thumbnails"].cast<py::list>();
        if (!thumbs.empty()) {
          py::dict bestThumb = thumbs[thumbs.size() - 1].cast<py::dict>();
          std::string thumbUrl = bestThumb["url"].cast<std::string>();
          resultData.thumbnailUrl = QString::fromStdString(thumbUrl);
        } else {
          qCWarning(YtWorker_l) << "Thumbnails empty";
        }
      } else {
        qCWarning(YtWorker_l) << "Unable to get member: " << "thumbnails" << ". From a result of ytmusicapi";
      }

      relatedTracks.append(resultData);
    }

    qCDebug(YtWorker_l) << "Related tracks fetched successfully.";
    Q_EMIT getRelatedTracksSuccess(relatedTracks);
  } catch (const py::error_already_set &e) {
    qCDebug(YtWorker_l) << "Related tracks failed to get\n" << e.what();
    Q_EMIT getRelatedTracksFailed(QString::fromStdString(e.what()));
  } catch (const std::exception &e) {
    qCDebug(YtWorker_l) << "Related tracks failed to get\n" << e.what();
    Q_EMIT getRelatedTracksFailed(QString::fromStdString(e.what()));
  }
}
