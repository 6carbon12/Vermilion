#include "MprisController.h"
#include "DesktopPlayer.h"
#include <QThread>
#include <QUrl>
#include <QtConcurrent/QtConcurrentRun>
#include <clocale>
#include <cstdio>
#include <mpv/client.h>
#include <qfuture.h>
#include <QDBusConnection>
#include <qloggingcategory.h>
#include <qtmetamacros.h>

namespace Core {
DesktopPlayer::DesktopPlayer() {
  setlocale(LC_NUMERIC, "C");
  mpvHandle = mpv_create();
  mpv_initialize(mpvHandle);
  mpv_set_option_string(mpvHandle, "video", "no");

  mprisController = new MprisController(this, this);

  QDBusConnection bus = QDBusConnection::sessionBus();
  bus.registerObject("/org/mpris/MediaPlayer2", mprisController);
  bus.registerService("org.mpris.MediaPlayer2.vermilion");

  mpv_observe_property(mpvHandle, 0, "pause", MPV_FORMAT_FLAG);
  mpv_observe_property(mpvHandle, 0, "time-pos/full", MPV_FORMAT_DOUBLE);
  mpv_observe_property(mpvHandle, 0, "paused-for-cache", MPV_FORMAT_FLAG);
  QFuture<void> future = QtConcurrent::run([this](mpv_handle *handle) { handleMpvState(handle); }, mpvHandle);
}

void DesktopPlayer::handleMpvState(mpv_handle *handle) {
  while (true) {
    mpv_event *event = mpv_wait_event(handle, -1);
    if (event->event_id == MPV_EVENT_SHUTDOWN) {
      break;
    }

    switch (event->event_id) {
    case MPV_EVENT_FILE_LOADED:
      isReady = true;
      state = PlayerState::Playing;
      Q_EMIT playerStateChanged();
      break;

    case MPV_EVENT_END_FILE: {
      mpv_event_end_file *end = (mpv_event_end_file *)event->data;
      if (end->reason == MPV_END_FILE_REASON_ERROR) {
        state = PlayerState::Error;
        isReady = false;
        Q_EMIT playerStateChanged();
      } else if (end->reason == MPV_END_FILE_REASON_EOF) {
        Q_EMIT requestNext();
      }
      break;
    }

    case MPV_EVENT_PROPERTY_CHANGE: {
      mpv_event_property *prop = (mpv_event_property *)event->data;
      if (strcmp(prop->name, "pause") == 0 && prop->format == MPV_FORMAT_FLAG && isReady) {
        bool isPaused = *(int *)prop->data;
        if (isPaused) {
          state = PlayerState::Paused;
        } else {
          state = PlayerState::Playing;
        }
        Q_EMIT playerStateChanged();
      } else if (strcmp(prop->name, "time-pos/full") == 0 && prop->format == MPV_FORMAT_DOUBLE) {
        double d_position = *static_cast<double *>(prop->data);
        position = std::lround((d_position) * 1000);
        Q_EMIT playerPositionChanged();
      } else if (strcmp(prop->name, "paused-for-cache") == 0 && prop->format == MPV_FORMAT_FLAG) {
        int is_paused_for_cache = *(int *)prop->data;
        if (is_paused_for_cache) {
          state = PlayerState::Buffering;
        } else {
          state = PlayerState::Playing;
        }
        Q_EMIT playerStateChanged();
      }
      break;
    }

    default:
      break;
    }
  }
}

void DesktopPlayer::play(const QString &url, const Core::Track &track) {
  QUrl mediaUrl(url);
  if (!mediaUrl.isValid()) {
    qWarning() << "Invalid URL provided to DesktopPlayer:" << url;
    return;
  }

  QByteArray utf8Url = url.toUtf8();
  const char *loadfileCmd[] = {"loadfile", utf8Url.constData(), NULL};
  currentTrack = track;

  int error = mpv_command(mpvHandle, loadfileCmd);
  if (error < 0) {
    qWarning() << "mpv loadfile failed with error:" << mpv_error_string(error);
  }

  Q_EMIT trackChanged();
}

Core::Track& DesktopPlayer::getCurrentTrack() {
  return currentTrack;
}

void DesktopPlayer::pause() { mpv_set_property_string(mpvHandle, "pause", "yes"); }

void DesktopPlayer::resume() { mpv_set_property_string(mpvHandle, "pause", "no"); }

void DesktopPlayer::stop() {
  const char *cmd[] = {"stop", NULL};
  mpv_command(mpvHandle, cmd);
}

Core::PlayerState::State DesktopPlayer::getPlayerState() { return state; };

void DesktopPlayer::seekTo(long positionMs) {
  std::string positionStr = std::to_string(positionMs / 1000.0);
  const char *seekCmd[] = {"seek", positionStr.c_str(), "absolute", NULL};
  mpv_command(mpvHandle, seekCmd);
  Q_EMIT seeked();
}

long DesktopPlayer::getCurrentPosition() { return position; }

long DesktopPlayer::getDuration() {
  double d_duration;
  mpv_get_property(mpvHandle, "duration/full", MPV_FORMAT_DOUBLE, &d_duration);
  duration = std::lround((d_duration) * 1000);
  return duration;
}

void DesktopPlayer::next() {
  Q_EMIT requestNext();
}

void DesktopPlayer::prev() {
  Q_EMIT requestPrev();
}
} // namespace Core
