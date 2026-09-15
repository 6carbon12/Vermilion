package io.github.x6carbon12.vermilion;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.os.Build;
import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;
import androidx.media3.common.AudioAttributes;
import androidx.media3.common.C;
import androidx.media3.common.MediaItem;
import androidx.media3.exoplayer.ExoPlayer;
import androidx.media3.session.MediaSession;
import androidx.media3.session.MediaSessionService;
import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import java.util.ArrayList;
import java.util.List;

public class PlaybackService extends MediaSessionService {
  private MediaSession mediaSession = null;
  private ExoPlayer player = null;
  private static final String CHANNEL_ID = "vermilion_playback_channel";
  private static final int NOTIFICATION_ID = 1001;

  @Override
  public void onCreate() {
    super.onCreate();

    createNotificationChannel();

    AudioAttributes audioAttributes = new AudioAttributes.Builder()
      .setContentType(C.AUDIO_CONTENT_TYPE_MUSIC)
      .setUsage(C.USAGE_MEDIA)
      .build();

    player = new ExoPlayer.Builder(this)
      .setAudioAttributes(audioAttributes, /* handleAudioFocus= */ true)
      .setHandleAudioBecomingNoisy(true)
      .build();

    // Maintain continuous foreground priority during track transitions and playback
    player.addListener(new androidx.media3.common.Player.Listener() {
      @Override
      public void onPlaybackStateChanged(int playbackState) {
        if (playbackState == androidx.media3.common.Player.STATE_BUFFERING ||
            playbackState == androidx.media3.common.Player.STATE_READY) {
          startServiceForegroundSafe("Vermilion", "Playing audio");
            }
      }
    });

    // Build the MediaSession with a custom callback implementing onPlaybackResumption
    mediaSession = new MediaSession.Builder(this, player)
      .setCallback(new MediaSession.Callback() {
        @Override
        public ListenableFuture<MediaSession.MediaItemsWithStartPosition> onPlaybackResumption(
            MediaSession session, MediaSession.ControllerInfo controller) {

          List<MediaItem> emptyList = new ArrayList<>();
          MediaSession.MediaItemsWithStartPosition resumptionState =
            new MediaSession.MediaItemsWithStartPosition(emptyList, C.INDEX_UNSET, C.TIME_UNSET);

          return Futures.immediateFuture(resumptionState);
            }
      })
    .build();
  }

  private void createNotificationChannel() {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
      NotificationChannel channel = new NotificationChannel(
          CHANNEL_ID,
          "Vermilion Playback",
          NotificationManager.IMPORTANCE_LOW
          );
      NotificationManager manager = getSystemService(NotificationManager.class);
      if (manager != null) {
        manager.createNotificationChannel(channel);
      }
    }
  }

  private void startServiceForegroundSafe(String title, String text) {
    Notification notification = new NotificationCompat.Builder(this, CHANNEL_ID)
      .setContentTitle(title)
      .setContentText(text)
      .setSmallIcon(android.R.drawable.ic_media_play)
      .setPriority(NotificationCompat.PRIORITY_LOW)
      .build();
    try {
      startForeground(NOTIFICATION_ID, notification);
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  @Nullable
  @Override
  public MediaSession onGetSession(MediaSession.ControllerInfo controllerInfo) {
    return mediaSession;
  }

  public ExoPlayer getPlayer() {
    return player;
  }

  @Override
  public void onDestroy() {
    if (mediaSession != null) {
      mediaSession.getPlayer().release();
      mediaSession.release();
      mediaSession = null;
      player = null;
    }
    super.onDestroy();
  }
}
