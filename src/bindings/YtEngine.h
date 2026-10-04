#pragma once

#include "Network.h"
#include "Track.h"
#include "YtWorker.h"
#include <QList>
#include <QObject>
#include <QString>
#include <QThread>

/// @brief Controls `YtWorker` in a separate thread and exposes it's function with some modifications.
///
/// Exposes `search` and `getRelatedTracks` as-is, `extractUrl` is exposed as `requestExtraction` which gives a playable
/// file directly.
class YtEngine : public QObject {
  Q_OBJECT
public:
  explicit YtEngine(QObject *parent = nullptr);
  ~YtEngine();

public Q_SLOTS:
  /// @brief Start extracting YouTube video URL and downloads it.
  /// @param url YouTube video URL to download.
  /// @see `extractionSuccess()`
  /// @see `extractionFailed()`
  void requestExtraction(const QString &url);

  /// @brief Does a YoutubeMusic search with give query.
  /// @param query Query to search for.
  /// @param maxResults Number of top results to choose.
  ///
  /// @see `searchSuccess()`
  /// @see `searchFailed()`
  void requestSearch(const QString &query, int maxResults);

  /// @brief Gets a list of tracks related to given track.
  /// @param url YouTube video URL of the track.
  ///
  /// @see `getRelatedTracksSuccess()`
  /// @see `getRelatedTracksFailed()`
  void getRelatedTracks(const QString &url);

Q_SIGNALS:

  /// @brief Emitted once a readable size of content is downloaded.
  /// @param url Path to downloaded file.
  /// @note Downloaded file is deleted when new URL extraction starts.
  void extractionSuccess(const QString &url);

  /// @brief Emitted if extracting streaming URL fails.
  /// @param error Error message generated.
  void extractionFailed(const QString &error);

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

private:
  QThread *workerThread;
  YtWorker *worker;
  Core::Network::StreamDownloader *downloader;

  /// @brief Internal signal only meant to communicate with internal objects using signals and slots, instead of normal
  /// calls.
  Q_SIGNAL void startWorkerInit();

  /// @brief Internal signal only meant to communicate with internal objects using signals and slots, instead of normal
  /// calls.
  Q_SIGNAL void startExtraction(const QString &url);

  /// @brief Internal signal only meant to communicate with internal objects using signals and slots, instead of normal
  /// calls.
  Q_SIGNAL void startSearch(const QString &query, int maxResults);

  /// @brief Internal signal only meant to communicate with internal objects using signals and slots, instead of normal
  /// calls.
  Q_SIGNAL void startGetRelatedTracks(const QString &url);
};
