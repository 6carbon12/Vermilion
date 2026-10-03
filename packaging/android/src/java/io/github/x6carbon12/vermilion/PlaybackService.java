package io.github.x6carbon12.vermilion;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;
import androidx.core.content.ContextCompat;
import androidx.media3.common.ForwardingPlayer;
import androidx.media3.common.MediaItem;
import androidx.media3.common.MediaMetadata;
import androidx.media3.common.Player;
import androidx.media3.common.PlaybackException;
import androidx.media3.exoplayer.ExoPlayer;
import androidx.media3.session.MediaSession;
import androidx.media3.session.MediaSessionService;
import androidx.media3.session.MediaStyleNotificationHelper;

public class PlaybackService extends MediaSessionService {
  private static PlaybackService instance;

  private MediaSession mediaSession;
  private ExoPlayer basePlayer;

  private CharSequence currentTitle = "Vermilion Engine";
  private CharSequence currentArtist = "Ready to play";

  private static final int NOTIFICATION_ID = 1001;
  private static final String CHANNEL_ID = "vermilion_playback_channel";

  public static void start(Context context) {
    ContextCompat.startForegroundService(context, new Intent(context, PlaybackService.class));
  }

  public static PlaybackService getInstance() {
    return instance;
  }

  public ExoPlayer getBasePlayer() {
    return basePlayer;
  }

  @Override
  public void onCreate() {
    super.onCreate();
    instance = this;

    createNotificationChannel();

    basePlayer = new ExoPlayer.Builder(this)
        .setHandleAudioBecomingNoisy(true)
        .build();

    basePlayer.addListener(new Player.Listener() {
      @Override
      public void onPlaybackStateChanged(int playbackState) {
        if (playbackState == Player.STATE_ENDED) {
          onNext();
        }
      }

      @Override
      public void onPlayerError(PlaybackException error) {
        if (error.getCause() instanceof java.io.EOFException ||
            error.errorCode == PlaybackException.ERROR_CODE_IO_UNSPECIFIED) {
          onNext();
        }
      }
    });

    mediaSession = new MediaSession.Builder(this, createWrappedPlayer(basePlayer)).build();
  }

  private native void onTrackEnded();

  private void safeOnTrackEnded() {
    try {
      onTrackEnded();
    } catch (UnsatisfiedLinkError e) {
      Log.w("Player", "Failed to call CPP native function 'onTrackEnded'");
    }
  }

  @Override
  public int onStartCommand(Intent intent, int flags, int startId) {
    Notification placeholderNotif = buildPlaybackNotification();
    startForeground(NOTIFICATION_ID, placeholderNotif);
    return super.onStartCommand(intent, flags, startId);
  }

  public void loadTrackInSession(String url, String title, String artist, String artUrl) {
    if (mediaSession == null || mediaSession.getPlayer() == null) {
      return;
    }

    currentTitle = (title != null && !title.isEmpty()) ? title : "Unknown Title";
    currentArtist = (artist != null && !artist.isEmpty()) ? artist : "Unknown Artist";

    MediaMetadata.Builder metaBuilder = new MediaMetadata.Builder()
        .setTitle(currentTitle)
        .setArtist(currentArtist);

    if (artUrl != null && !artUrl.isEmpty()) {
      metaBuilder.setArtworkUri(Uri.parse(artUrl));
    }

    MediaItem mainItem = new MediaItem.Builder()
        .setUri(Uri.parse(url))
        .setMediaMetadata(metaBuilder.build())
        .build();

    MediaItem dummyNextItem = new MediaItem.Builder()
        .setUri(Uri.parse(url))
        .setMediaMetadata(new MediaMetadata.Builder().setTitle("Next Track Placeholder").build())
        .build();

    mediaSession.getPlayer().setMediaItems(java.util.Arrays.asList(mainItem, dummyNextItem));
    mediaSession.getPlayer().prepare();

    NotificationManager manager = getSystemService(NotificationManager.class);
    if (manager != null) {
      manager.notify(NOTIFICATION_ID, buildPlaybackNotification());
    }
  }

  private Notification buildPlaybackNotification() {
    boolean isPlaying = mediaSession != null && mediaSession.getPlayer() != null
        && mediaSession.getPlayer().getPlayWhenReady();

    Bitmap genericAlbumArt = BitmapFactory.decodeResource(getResources(), android.R.drawable.ic_menu_gallery);

    NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
        .setContentTitle(currentTitle)
        .setContentText(currentArtist)
        .setSmallIcon(android.R.drawable.ic_media_play)
        .setLargeIcon(genericAlbumArt)
        .setPriority(NotificationCompat.PRIORITY_LOW)
        .setVisibility(NotificationCompat.VISIBILITY_PUBLIC)
        .setOngoing(true);

    if (mediaSession != null) {
      builder.setStyle(new MediaStyleNotificationHelper.MediaStyle(mediaSession)
          .setShowActionsInCompactView(0, 1, 2));

      builder
          .addAction(android.R.drawable.ic_media_previous, "Previous",
              buildMediaIntent(Player.COMMAND_SEEK_TO_PREVIOUS))
          .addAction(isPlaying ? android.R.drawable.ic_media_pause : android.R.drawable.ic_media_play,
              isPlaying ? "Pause" : "Play", buildMediaIntent(Player.COMMAND_PLAY_PAUSE))
          .addAction(android.R.drawable.ic_media_next, "Next", buildMediaIntent(Player.COMMAND_SEEK_TO_NEXT));
    }

    return builder.build();
  }

  private PendingIntent buildMediaIntent(int command) {
    return PendingIntent.getService(this, command, new Intent(), PendingIntent.FLAG_IMMUTABLE);
  }

  private void createNotificationChannel() {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
      NotificationChannel channel = new NotificationChannel(
          CHANNEL_ID, "Vermilion Playback", NotificationManager.IMPORTANCE_LOW);
      channel.setLockscreenVisibility(Notification.VISIBILITY_PUBLIC);
      NotificationManager manager = getSystemService(NotificationManager.class);
      if (manager != null) {
        manager.createNotificationChannel(channel);
      }
    }
  }

  private ForwardingPlayer createWrappedPlayer(ExoPlayer player) {
    return new ForwardingPlayer(player) {
      @Override
      public void play() {
        onPlay();
      }

      @Override
      public void pause() {
        onPause();
      }

      @Override
      public void seekToNext() {
        onNext();
      }

      @Override
      public void seekToPrevious() {
        onPrev();
      }

      @Override
      public boolean hasNextMediaItem() {
        return true;
      }

      @Override
      public boolean hasPreviousMediaItem() {
        return true;
      }

      @Override
      public MediaMetadata getMediaMetadata() {
        return new MediaMetadata.Builder()
            .setTitle(currentTitle)
            .setArtist(currentArtist)
            .build();
      }

      @Override
      public Player.Commands getAvailableCommands() {
        return super.getAvailableCommands().buildUpon()
            .add(Player.COMMAND_SEEK_TO_NEXT)
            .add(Player.COMMAND_SEEK_TO_PREVIOUS)
            .add(Player.COMMAND_CHANGE_MEDIA_ITEMS)
            .add(Player.COMMAND_PLAY_PAUSE)
            .build();
      }
    };
  }

  @Nullable
  @Override
  public MediaSession onGetSession(MediaSession.ControllerInfo controllerInfo) {
    return mediaSession;
  }

  @Override
  public void onDestroy() {
    if (mediaSession != null) {
      mediaSession.release();
      mediaSession = null;
    }
    if (basePlayer != null) {
      basePlayer.release();
      basePlayer = null;
    }
    instance = null;
    super.onDestroy();
  }

  private native void onPlay();

  private native void onPause();

  private native void onNext();

  private native void onPrev();
}
