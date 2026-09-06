#pragma once

#include <QNetworkReply>
#include <QTemporaryFile>
#include <QUrl>
#include <QObject>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace Core::Network {
class StreamDownloader : public QObject {
  Q_OBJECT
public:
  explicit StreamDownloader(QObject *parent = nullptr);
  ~StreamDownloader();

  Q_INVOKABLE void startDownload(const QString &url, const QMap<QByteArray, QByteArray> headersMap);
  Q_INVOKABLE void stop();

Q_SIGNALS:
  void bufferReady(const QString &localFileUrl);
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

  const qint64 BUFFER_THRESHOLD = 128 * 1024; /* 128 KB */
  const qint64 CHUNK_SIZE = 10 * 1024 * 1024; /* 10 MB */

  void requestNextChunk();

};
} // namespace Core::Network
