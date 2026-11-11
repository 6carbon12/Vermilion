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
#include <qnamespace.h>
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
#ifdef Q_OS_ANDROID
      QString nativeLibDir = QCoreApplication::applicationDirPath();
      py::module_ sys = py::module_::import("sys");
      sys.attr("path").attr("append")(nativeLibDir.toStdString());

      // Invalidate caches to force a rescan of modules.
      py::module_::import("importlib").attr("invalidate_caches")();
#endif

      py::dict ydlOpts = getGeneralYdlOpts();
      yt_dlp = py::module_::import("yt_dlp");
      YoutubeDL = yt_dlp.attr("YoutubeDL");
      yt_dlp = py::module_::import("ytmusicapi");
      YTMusic = yt_dlp.attr("YTMusic")();
    } catch (py::error_already_set &e) {
      qCWarning(YtWorker_l) << "Initializtion failed.";
      qCWarning(YtWorker_l) << e.what();
      Q_EMIT initFailed(PyHelper::Error({PyHelper::ErrorReason::ImportFailed, e.what()}));
    }
  }
}

void YtWorker::search(const QString &query, int maxResults) {
  using namespace pybind11::literals;
  py::gil_scoped_acquire acquire;

  QList<Core::Track> resultsList;
  try {
    py::object results = YTMusic.attr("search")(query.toStdString(), "filter"_a = "songs", "limit"_a = maxResults);

    for (py::handle result : results) {
      py::dict resultDict = result.cast<py::dict>();
      Core::Track resultData = getTrackFromDict(resultDict);
      resultsList.append(resultData);
    }

    Q_EMIT searchSuccess(resultsList);
    qCDebug(YtWorker_l) << "Search successful for:" << query;
  } catch (const py::error_already_set &e) {
    qCDebug(YtWorker_l) << "Search failed" << e.what();
    Q_EMIT searchFailed(QString::fromStdString(e.what()));
  } catch (const std::exception &e) {
    qCDebug(YtWorker_l) << "Search failed" << e.what();
    Q_EMIT searchFailed(QString::fromStdString(e.what()));
  }
}

void YtWorker::extractUrl(const QString &url) {
  m_pendingExtractionUrl = url;

  if (!m_isExtracting) {
    m_isExtracting = true;
    QMetaObject::invokeMethod(this, "extractUrlImpl", Qt::QueuedConnection);
  }
}

void YtWorker::getRelatedTracks(const QString &url) {
  m_pendingRelatedTracksUrl = url;

  if (!m_isGettingRelatedTracks) {
    m_isGettingRelatedTracks = true;
    QMetaObject::invokeMethod(this, "getRelatedTracksImpl", Qt::QueuedConnection);
  }
}

void YtWorker::extractUrlImpl() {
  if (m_pendingExtractionUrl.isEmpty()) {
    m_isExtracting = false;
    return;
  }

  QString urlToProcess = m_pendingExtractionUrl;
  m_pendingExtractionUrl.clear();

  QString audioURL;
  QMap<QByteArray, QByteArray> headersMap;
  bool success = false;
  QString errorString;

  // GIL scope.
  {
    using namespace pybind11::literals;
    py::gil_scoped_acquire acquire;

    try {
      py::dict ydlOpts = getGeneralYdlOpts();
      py::object ydl = YoutubeDL(ydlOpts);
      py::object info = ydl.attr("extract_info")(urlToProcess.toStdString(), "download"_a = false);

      if (info.contains("url") && !info["url"].is_none()) {
        audioURL = QString::fromStdString(info["url"].cast<std::string>());

        if (info.contains("http_headers")) {
          py::dict headers = info["http_headers"].cast<py::dict>();
          for (auto item : headers) {
            headersMap.insert(QByteArray::fromStdString(item.first.cast<std::string>()),
                              QByteArray::fromStdString(item.second.cast<std::string>()));
          }
        }
        success = true;
      } else {
        errorString = "'url' not found in the info object.";
      }
    } catch (const std::exception &e) {
      errorString = QString::fromStdString(e.what());
    }
  }

  // Force the thread to process any extractUrl() calls that were
  // queued by the UI thread while Python was blocking this thread.
  QCoreApplication::processEvents();

  // Check the url only AFTER the requests have been processed.
  if (m_pendingExtractionUrl.isEmpty()) {
    if (success) {
      Q_EMIT extractSuccess(audioURL, headersMap);
      qCDebug(YtWorker_l) << "Extraction completed.";
    } else {
      Q_EMIT extractFailed(errorString);
      qCWarning(YtWorker_l) << "Extraction failed:" << errorString;
    }
  } else {
    qCDebug(YtWorker_l) << "Extraction superseded by a newer request. Discarding result.";
  }

  if (!m_pendingExtractionUrl.isEmpty()) {
    qCDebug(YtWorker_l) << "Extraction superseded by a newer request. Discarding result.";
    QMetaObject::invokeMethod(this, "extractUrlImpl", Qt::QueuedConnection);
  } else {
    m_isExtracting = false;
  }
}

void YtWorker::getRelatedTracksImpl() {
  if (m_pendingRelatedTracksUrl.isEmpty()) {
    m_isGettingRelatedTracks = false;
    return;
  }

  QString urlToProcess = m_pendingRelatedTracksUrl;
  m_pendingRelatedTracksUrl.clear();

  QRegularExpression regex(QStringLiteral("v=([a-zA-Z0-9_-]{11})"));
  QRegularExpressionMatch match = regex.match(urlToProcess);

  if (!match.hasMatch() || match.captured(1).length() != 11) {
    // Process events here just in case a new request was fired while we were verifying regex
    QCoreApplication::processEvents();
    if (m_pendingRelatedTracksUrl.isEmpty()) {
      Q_EMIT getRelatedTracksFailed("Invalid YouTube URL or Video ID.");
    }
  } else {
    QString videoId = match.captured(1);
    std::string videoIdStd = videoId.toStdString();

    QList<Core::Track> relatedTracks;
    bool success = false;
    QString errorString;

    // GIL scope.
    {
      py::gil_scoped_acquire acquire;
      try {
        auto result = PyHelper::loadScript(":/python/yt_helpers.py");

        if (!result) {
          errorString = result.error().debugContext;
        } else {
          py::object scope = py::module_::import("__main__").attr("__dict__");
          py::object getWatchPlaylistWithExplicit = scope["get_watch_playlist_normalized"];
          py::object watchPlaylist = getWatchPlaylistWithExplicit(YTMusic, videoIdStd);

          py::list tracks = watchPlaylist["tracks"].cast<py::list>();
          for (py::handle track : tracks) {
            relatedTracks.append(getTrackFromDict(track.cast<py::dict>()));
          }
          success = true;
        }
      } catch (const std::exception &e) {
        errorString = QString::fromStdString(e.what());
      }
    }

    // Force the thread to process any getRelatedTracks() calls that were
    // queued by the UI thread while Python was blocking this thread.
    QCoreApplication::processEvents();

    if (m_pendingRelatedTracksUrl.isEmpty()) {
      if (success) {
        qCDebug(YtWorker_l) << "Related tracks fetched successfully.";
        Q_EMIT getRelatedTracksSuccess(relatedTracks);
      } else {
        qCCritical(YtWorker_l) << "Failed to fetch related tracks:" << errorString;
        Q_EMIT getRelatedTracksFailed(errorString);
      }
    } else {
      qCDebug(YtWorker_l) << "Related tracks superseded by a newer request. Discarding result.";
    }
  }

  if (!m_pendingRelatedTracksUrl.isEmpty()) {
    qCDebug(YtWorker_l) << "Related tracks superseded by a newer request. Discarding result.";
    QMetaObject::invokeMethod(this, "getRelatedTracksImpl", Qt::QueuedConnection);
  } else {
    m_isGettingRelatedTracks = false;
  }
}

Core::Track YtWorker::getTrackFromDict(py::object dict) {
  Core::Track track;

  track.durationString = getStrMember(dict, "duration");
  track.id = getStrMember(dict, "videoId");
  track.title = getStrMember(dict, "title");
  track.url = "https://youtube.com/watch?v=" + getStrMember(dict, "videoId");
  track.views = getStrMember(dict, "views");

  if (dict.contains("thumbnails") && !dict["thumbnails"].is_none()) {
    py::list thumbs = dict["thumbnails"].cast<py::list>();
    if (!thumbs.empty()) {
      py::dict bestThumb = thumbs[thumbs.size() - 1].cast<py::dict>();
      std::string thumbUrl = bestThumb["url"].cast<std::string>();
      track.coverArtUrl = QString::fromStdString(thumbUrl);
    } else {
      qCWarning(YtWorker_l) << "Thumbnails empty";
    }
  } else {
    qCWarning(YtWorker_l) << "Unable to get member 'thumbnails' from a result of ytmusicapi";
  }

  if (dict.contains("isExplicit") && !dict["isExplicit"].is_none()) {
    track.isExplict = dict["isExplicit"].cast<bool>();
  } else {
    qCWarning(YtWorker_l) << "Unable to get member 'isExplict' from a result of ytmusicapi";
  }

  if (dict.contains("duration_seconds") && !dict["duration_seconds"].is_none()) {
    track.duration = dict["duration_seconds"].cast<long>() * 1000;
  } else {
    qCWarning(YtWorker_l) << "Unable to get member 'duration' from a result of ytmusicapi";
  }

  if (dict.contains("artists") && !dict["artists"].is_none()) {
    py::list artists = dict["artists"].cast<py::list>();
    int i = 0;
    if (!artists.empty()) {
      for (auto &artist : artists) {
        py::dict artistDict = artist.cast<py::dict>();
        track.artists[i].name = getStrMember(artistDict, "name");
        track.artists[i].id = getStrMember(artistDict, "id");
        i++;
      }
      track.artistsNum = i;
    } else {
      qCWarning(YtWorker_l) << "artists array empty";
    }
  } else {
    qCWarning(YtWorker_l) << "Unable to get member 'artists' from a result of ytmusicapi";
  }
  return track;
}

QString YtWorker::getStrMember(py::object dict, const char *member) {
  if (dict.contains(member) && !dict[member].is_none()) {
    return QString::fromStdString(dict[member].cast<std::string>());
  } else {
    qCWarning(YtWorker_l) << "Unable to get member" << member << "from a result of ytmusicapi";
    return "";
  }
}
