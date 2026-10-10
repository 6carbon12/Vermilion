#pragma once
#include "DesktopPlayer.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include <qtmetamacros.h>

namespace Core {
class DesktopPlayer;
}
class MprisController : public QObject {
  using DP = Core::DesktopPlayer;
  Q_OBJECT

  // MPRIS Properties
  Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
  Q_PROPERTY(QVariantMap Metadata READ metadata)
  Q_PROPERTY(qlonglong Position READ position)
  Q_PROPERTY(bool CanGoNext READ canGoNext)
  Q_PROPERTY(bool CanGoPrevious READ canGoPrevious)
  Q_PROPERTY(bool CanPlay READ canPlay)
  Q_PROPERTY(bool CanPause READ canPause)
  Q_PROPERTY(bool CanSeek READ canSeek)
  Q_PROPERTY(bool CanControl READ canControl)

public:
  explicit MprisController(DP *player, QObject *parent = nullptr);

  QString playbackStatus() const;
  QVariantMap metadata() const;
  qlonglong position() const;

public Q_SLOTS:
  void Play();
  void Pause();
  void PlayPause();
  void Next();
  void Previous();
  void Stop();
  void Quit();
  void Raise();

  bool canGoNext() const { return true; }
  bool canGoPrevious() const { return true; }
  bool canPlay() const { return true; }
  bool canPause() const { return true; }
  bool canSeek() const { return true; }
  bool canControl() const { return true; }

private Q_SLOTS:
  void onPlayerStateChanged();
  void onPlayerMetadataChanged();
  void onPlayerSeeked();

private:
  void emitPropertiesChanged(const QString &propertyName, const QVariant &value);
  QString getVideoId(QString ytURL) const;

  DP *m_player; // Non-owning reference
};
