#pragma once

#include "PyHelper.h"
#include "Track.h"
#include <QObject>
#include <pybind11/embed.h>
#include <qtmetamacros.h>

namespace py = pybind11;
/// @brief Controls and exposes specific functions of yt_dlp and ytmusicapi python module
///
/// Uses pybind11 to interact with python and expose Python functionality to c++.
class YtWorker : public QObject {
  Q_OBJECT
private:
  py::object yt_dlp; ///< yt_dlp module
  py::object ytmusicapi; ///< ytmusicapi module
  py::object YoutubeDL; ///< YoutubeDL object from yt_dlp
  py::object YTMusic; ///< YTMusic object from ytmusicapi

  /// @brief Gives general options used for YoutubeDL initialization
  ///
  /// Created as a way to centrally manage YoutubeDL configuration.
  /// Options can be modified further wherever fit.
  py::dict getGeneralYdlOpts();

public:
  explicit YtWorker(QObject *parent = nullptr);
  ~YtWorker();
public Q_SLOTS:
  /// @brief Initializes worker.
  ///
  /// Installs needed python modules.
  /// Setups `yt_dlp` and `ytmusicapi`
  void init();

  /// @brief Gets the YouTube streaming URL from normal YouTube video URL.
  /// @param url URL of the video to extract from.
  ///
  /// @see `extractSuccess()`
  /// @see `extractFailed()`
  void extractUrl(const QString &url);

  /// @brief Does a YoutubeMusic search with give query.
  /// @param query Query to search for.
  /// @param maxResults Number of top results to choose.
  ///
  /// @see `searchSuccess()`
  /// @see `searchFailed()`
  void search(const QString &query, int maxResults = 5);


  /// @brief Gets a list of tracks related to given track.
  /// @param url YouTube video URL of the track.
  ///
  /// @see `getRelatedTracksSuccess()`
  /// @see `getRelatedTracksFailed()`
  void getRelatedTracks(const QString &url);
Q_SIGNALS:
  /// @brief Emitted if any error occurs while initialization.
  void initFailed(PyHelper::Error error);

  /// @brief Emitted once streaming URL is successfully extracted.
  /// @param url Streaming URL.
  /// @param headers Preferred headers to be use while fetching `url`.
  void extractSuccess(const QString &url, const QMap<QByteArray, QByteArray> &headers);

  /// @brief Emitted if extracting streaming URL fails.
  /// @param error Error message generated.
  void extractFailed(const QString &error);

  /// @brief Emitted when search successfully completes.
  /// @param results Track from search result.
  void searchSuccess(const QList<Core::Track> &results);

  /// @brief Emitted when search fails.
  /// @param error Error message generated.
  void searchFailed(const QString &error);

  /// @brief Emitted when related tracks are fetched successfully.
  /// @param tracks related Tracks fetched.
  void getRelatedTracksSuccess(const QList<Core::Track> &tracks);

  /// @brief Emitted when fetching related tracks fails.
  /// @param error Error message generated.
  void getRelatedTracksFailed(const QString &error);
};
