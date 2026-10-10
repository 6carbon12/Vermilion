// MprisController.cpp
#include "MprisController.h"
#include "Player.h"
#include "PlayerState.h"
#include "mpris2_adaptor.h"
#include "player_adaptor.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QCoreApplication>

MprisController::MprisController(DP *player, QObject *parent) : QObject(parent), m_player(player) {
  new Mpris2Adaptor(this);
  new PlayerAdaptor(this);

  connect(m_player, &Core::Player::playerStateChanged, this, &MprisController::onPlayerStateChanged);
  connect(m_player, &DP::trackChanged, this, &MprisController::onPlayerMetadataChanged);
  connect(m_player, &DP::seeked, this, &MprisController::onPlayerSeeked);
}

QString MprisController::playbackStatus() const {
  switch (m_player->getPlayerState()) {
  case Core::PlayerState::State::Playing:
    return "Playing";
  case Core::PlayerState::State::Paused:
    return "Paused";
  default:
    return "Stopped";
  }
}

QVariantMap MprisController::metadata() const {
  QVariantMap metadata;
  Core::Track &track = m_player->getCurrentTrack();
  QString videoId = getVideoId(track.url);
  QString trackIdStr;
  if (videoId.isEmpty()) {
    trackIdStr = QString("/org/vermilion/track/NoTrack");
  } else {
    videoId.replace(QRegularExpression("[^a-zA-Z0-9_]"), "_");
    trackIdStr = QString("/org/vermilion/track/%1").arg(videoId);
  }

  metadata.insert("mpris:trackid", QVariant::fromValue(QDBusObjectPath(trackIdStr)));
  metadata.insert("xesam:title", track.title);
  metadata.insert("xesam:artist", QStringList() << track.artist);
  qlonglong lengthMicroseconds = m_player->getDuration() * 1000LL;
  metadata.insert("mpris:length", lengthMicroseconds);
  if (!track.thumbnailUrl.isEmpty()) {
    metadata.insert("mpris:artUrl", track.thumbnailUrl);
  }
  metadata.insert("xesam:album", "");

  return metadata;
}

QString MprisController::getVideoId(QString ytURL) const {
  QRegularExpression regex("v=([a-zA-Z0-9_-]{11})");

  QRegularExpressionMatch match = regex.match(ytURL);
  QString videoId{};
  if (match.hasMatch()) {
    videoId = match.captured(1);
    if (videoId.length() != 11) {
      return "";
    }
  } else {
    return "";
  }

  return videoId;
}

qlonglong MprisController::position() const {
  return m_player->getCurrentPosition() * 1000;
}

void MprisController::Play() { m_player->resume(); }
void MprisController::Pause() { m_player->pause(); }
void MprisController::PlayPause() {
  m_player->getPlayerState() == Core::PlayerState::State::Playing ? m_player->pause() : m_player->resume();
}
void MprisController::Next() { m_player->next(); }
void MprisController::Previous() { m_player->prev(); }

void MprisController::onPlayerStateChanged() { emitPropertiesChanged("PlaybackStatus", playbackStatus()); }
void MprisController::onPlayerMetadataChanged() { emitPropertiesChanged("Metadata", metadata()); }
void MprisController::onPlayerSeeked() {
  qint64 positionMs = m_player->getCurrentPosition();
  QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", "Seeked");
  msg << (positionMs * 1000); // Microseconds
  QDBusConnection::sessionBus().send(msg);
}

void MprisController::emitPropertiesChanged(const QString &propertyName, const QVariant &value) {
  QVariantMap changedProps;
  changedProps.insert(propertyName, value);

  QDBusMessage msg =
      QDBusMessage::createSignal("/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "PropertiesChanged");

  msg << "org.mpris.MediaPlayer2.Player" << changedProps << QStringList();
  QDBusConnection::sessionBus().send(msg);
}

void MprisController::Stop() {
  m_player->pause(); 
}

void MprisController::Quit() {
  QCoreApplication::quit();
}

void MprisController::Raise() {}
