#include "Network.h"
#include <qlogging.h>
#include <qloggingcategory.h>
#include <qtmetamacros.h>

Q_LOGGING_CATEGORY(network, "Network");

namespace Core::Network {
StreamDownloader::StreamDownloader(QObject *parent)
    : QObject(parent), manager(new QNetworkAccessManager(this)), reply(nullptr), tempFile(nullptr) {}

StreamDownloader::~StreamDownloader() { stop(); }

void StreamDownloader::startDownload(const QString &url, const QMap<QByteArray, QByteArray> headers) {
  stop();

  tempFile = new QTemporaryFile(this);
  if (!tempFile->open()) {
    qCWarning(network) << "Failed to create temporary file to download.";
    Q_EMIT downloadFailed("Failed to create temporary file.", QNetworkReply::NetworkError::NoError);
    return;
  }

  bufferSignaled = false;
  currentUrl = url;
  currentHeaders = headers;
  currentOffset = 0;

  requestNextChunk();
}

void StreamDownloader::requestNextChunk() {
  QNetworkRequest request((QUrl(currentUrl)));

  for (auto it = currentHeaders.constBegin(); it != currentHeaders.constEnd(); ++it) {
    request.setRawHeader(it.key(), it.value());
  }

  QByteArray range = QString("bytes=%1-%2").arg(currentOffset).arg(currentOffset + CHUNK_SIZE - 1).toUtf8();
  request.setRawHeader("Range", range);

  reply = manager->get(request);

  connect(reply, &QNetworkReply::readyRead, this, &StreamDownloader::onReadyRead);
  connect(reply, &QNetworkReply::finished, this, &StreamDownloader::onFinished);
  connect(reply, &QNetworkReply::errorOccurred, this, &StreamDownloader::onErrorOccurred);
}

void StreamDownloader::onReadyRead() {
  if (!reply || !tempFile) {
    return;
  }

  tempFile->write(reply->readAll());
  if (!bufferSignaled && tempFile->size() >= BUFFER_THRESHOLD) {
    tempFile->flush();
    bufferSignaled = true;
    qCDebug(network) << "Buffer ready to read.";
    Q_EMIT bufferReady(QUrl::fromLocalFile(tempFile->fileName()).toString());
  }
}

void StreamDownloader::onFinished() {
  if (!reply) {
    return;
  }

  int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  if (httpStatus == 416) {
    qCDebug(network) << "Download completely finished (EOF).";
    reply->deleteLater();
    reply = nullptr;
    return;
  }

  if (reply->error() == QNetworkReply::NoError) {
    // Edgecase where size of file is less than buffer.
    if (!bufferSignaled) {
      bufferSignaled = true;
      Q_EMIT bufferReady(QUrl::fromLocalFile(tempFile->fileName()).toString());
    }

    qint64 contentLength = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    reply->deleteLater();
    reply = nullptr;

    if (contentLength == CHUNK_SIZE) {
      currentOffset += CHUNK_SIZE;
      requestNextChunk();
    } else {
      // The final chunk will be smaller than CHUNK_SIZE if we reach the end
      qCDebug(network) << "Download completely finished.";
    }
  }
}

void StreamDownloader::onErrorOccurred(const QNetworkReply::NetworkError &code) {
  if (reply) {
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (httpStatus == 416) {
      return; // Safe to ignore since it is handled by onFinished
    }
    Q_EMIT downloadFailed(reply->errorString(), code);
    qCWarning(network) << "Failed to download.";
    qCWarning(network) << reply->errorString() << code;
  }
}

void StreamDownloader::stop() {
  if (reply) {
    qCDebug(network) << "Stopped downloading.";
    reply->abort();
    reply->deleteLater();
    reply = nullptr;
  }
  if (tempFile) {
    tempFile->deleteLater();
    tempFile = nullptr;
  }
}
} // namespace Core::Network
