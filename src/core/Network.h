#pragma once

#include <QNetworkReply>
#include <QTemporaryFile>
#include <QUrl>
#include <QObject>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace Core::Network {
/// @brief A buffered file downloader.
///
/// `StreamDownloader` downloads contents of a give URL to a temporay file.
/// This file is instantly available to read from, once `BUFFER_THRESHOLD` (128 KB) is reached.
///
/// @note the created temporay file is deleted once a new download starts.
class StreamDownloader : public QObject {
  Q_OBJECT
public:
  explicit StreamDownloader(QObject *parent = nullptr);
  ~StreamDownloader();

  /// @param url URL to the resource to be downloaded.
  /// @param headersMap HTTP headers to be used for downloading the contents.
  Q_INVOKABLE void startDownload(const QString &url, const QMap<QByteArray, QByteArray> headersMap);
  /// @brief Stop download and deletes temporay file.
  Q_INVOKABLE void stop();

Q_SIGNALS:
  /// @brief Emitted when file is ready to be read.
  void bufferReady(const QString &localFileUrl);
  /// @brief Emitted when any error occurs.
  /// @note If the error is non-network related error is set to NetworkError::NoError.
  void downloadFailed(const QString &errorStr, const QNetworkReply::NetworkError &error);

private Q_SLOTS:
  void onReadyRead();
  void onFinished();
  void onErrorOccurred(const QNetworkReply::NetworkError& code);

private:
  QNetworkAccessManager *manager;
  QNetworkReply *reply;
  QTemporaryFile *tempFile;
  bool bufferSignaled;
  QString currentUrl;
  QMap<QByteArray, QByteArray> currentHeaders;
  qint64 currentOffset;

  const qint64 BUFFER_THRESHOLD = 128 * 1024; ///< Size of file before emitting `bufferReady`.
  const qint64 CHUNK_SIZE = 10 * 1024 * 1024; ///< Bytes of data to be read from each HTTP request.

  void requestNextChunk();

};
} // namespace Core::Network
