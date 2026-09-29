package io.github.x6carbon12.vermilion;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
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
import androidx.core.graphics.drawable.IconCompat;
import androidx.media3.common.ForwardingPlayer;
import androidx.media3.common.MediaItem;
import androidx.media3.common.MediaMetadata;
import androidx.media3.common.Player;
import androidx.media3.exoplayer.ExoPlayer;
import androidx.media3.session.CommandButton;
import androidx.media3.session.MediaNotification;
import androidx.media3.session.MediaSession;
import androidx.media3.session.MediaSessionService;
import androidx.media3.session.MediaStyleNotificationHelper;

import com.google.common.collect.ImmutableList;

public class PlaybackService extends MediaSessionService {
  private static PlaybackService instance;
  private static Runnable onReadyCallback;

  private MediaSession mediaSession;
  private ExoPlayer basePlayer;

  private CharSequence currentTitle = "Vermilion Engine";
  private CharSequence currentArtist = "Unknown Artist";

  private static final int NOTIFICATION_ID = 1001;
  private static final String CHANNEL_ID = "vermilion_playback_channel";

  public static void start(Context context, Runnable onReady) {
    onReadyCallback = onReady;
    ContextCompat.startForegroundService(context, new Intent(context, PlaybackService.class));
  }

  public static PlaybackService getInstance() { return instance; }
  public ExoPlayer getBasePlayer() { return basePlayer; }

  public void loadTrackInSession(String url, String title, String artist, String artUrl) {
    if (mediaSession != null && mediaSession.getPlayer() != null) {
      currentTitle = (title != null && !title.isEmpty()) ? title : "Vermilion Engine";
      currentArtist = (artist != null && !artist.isEmpty()) ? artist : "Unknown Artist";

      // 1. Build the main active track item
      MediaMetadata metadata = new MediaMetadata.Builder()
        .setTitle(currentTitle)
        .setArtist(currentArtist)
        .build();

      if (artUrl != null && !artUrl.isEmpty()) {
        metadata = metadata.buildUpon().setArtworkUri(Uri.parse(artUrl)).build();
      }

      MediaItem mainItem = new MediaItem.Builder()
        .setUri(Uri.parse(url))
        .setMediaMetadata(metadata)
        .build();

      // 2. Build a dummy next item so ExoPlayer's timeline has a valid forward target
      MediaItem dummyNextItem = new MediaItem.Builder()
        .setUri(Uri.parse(url)) // Can reuse the same URI or a silent placeholder
        .setMediaMetadata(new MediaMetadata.Builder().setTitle("Next Track Placeholder").build())
        .build();

      // 3. Set them as a playlist collection
      mediaSession.getPlayer().setMediaItems(java.util.Arrays.asList(mainItem, dummyNextItem));
      mediaSession.getPlayer().prepare();
      mediaSession.getPlayer().play();

      if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
        Bitmap genericAlbumArt = BitmapFactory.decodeResource(getResources(), android.R.drawable.ic_menu_gallery);

        NotificationCompat.Action prevAction = new NotificationCompat.Action(
            android.R.drawable.ic_media_previous, "Previous", null);
        NotificationCompat.Action nextAction = new NotificationCompat.Action(
            android.R.drawable.ic_media_next, "Next", null);
        NotificationCompat.Action playPauseAction = new NotificationCompat.Action(
            android.R.drawable.ic_media_pause, "Pause", null);

        Notification updatedNotif = new NotificationCompat.Builder(this, CHANNEL_ID)
          .setContentTitle(currentTitle)
          .setContentText(currentArtist)
          .setSmallIcon(android.R.drawable.ic_media_play)
          .setLargeIcon(genericAlbumArt)
          .setPriority(NotificationCompat.PRIORITY_DEFAULT)
          .setVisibility(NotificationCompat.VISIBILITY_PUBLIC)
          .setOngoing(true)
          .addAction(prevAction)
          .addAction(playPauseAction)
          .addAction(nextAction)
          .setStyle(new MediaStyleNotificationHelper.MediaStyle(mediaSession)
              .setShowActionsInCompactView(0, 1, 2))
          .build();

        NotificationManager manager = getSystemService(NotificationManager.class);
        if (manager != null) {
          manager.notify(NOTIFICATION_ID, updatedNotif);
        }
      }

      Log.d("VermilionDebug", "Playlist loaded with dummy item, next button unlocked for: " + currentTitle);
    }
  }

  @Override
  public void onCreate() {
    super.onCreate();
    instance = this;

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
      NotificationChannel channel = new NotificationChannel(
          CHANNEL_ID, "Vermilion Playback", NotificationManager.IMPORTANCE_DEFAULT);
      channel.setLockscreenVisibility(Notification.VISIBILITY_PUBLIC);
      NotificationManager manager = getSystemService(NotificationManager.class);
      if (manager != null) manager.createNotificationChannel(channel);
    }

    basePlayer = new ExoPlayer.Builder(this)
      .setHandleAudioBecomingNoisy(true)
      .build();

    ForwardingPlayer wrappedPlayer = new ForwardingPlayer(basePlayer) {
      @Override public void play() { onPlay(); }
      @Override public void pause() { onPause(); }
      @Override public void seekToNext() {
      Log.d("VermilionDebug", "Called seekToNext");
      onNext(); 
      }
      @Override public void seekToPrevious() { onPrev(); }
      @Override public boolean hasNextMediaItem() { return true; }
      @Override public boolean hasPreviousMediaItem() { return true; }

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
    mediaSession = new MediaSession.Builder(this, wrappedPlayer).build();

    setMediaNotificationProvider(new MediaNotification.Provider() {
      @Override
      public MediaNotification createNotification(
          MediaSession session,
          ImmutableList<CommandButton> customLayout,
          MediaNotification.ActionFactory actionFactory,
          Callback callback) {

        NotificationCompat.Action prevAction = actionFactory.createMediaAction(
            session, IconCompat.createWithResource(PlaybackService.this, android.R.drawable.ic_media_previous),
            "Previous", Player.COMMAND_SEEK_TO_PREVIOUS);

        NotificationCompat.Action nextAction = actionFactory.createMediaAction(
            session, IconCompat.createWithResource(PlaybackService.this, android.R.drawable.ic_media_next),
            "Next", Player.COMMAND_SEEK_TO_NEXT);

        Player activePlayer = session.getPlayer();
        boolean isPlaying = activePlayer != null && activePlayer.getPlayWhenReady();

        NotificationCompat.Action playPauseAction = actionFactory.createMediaAction(
            session, IconCompat.createWithResource(PlaybackService.this, 
              isPlaying ? android.R.drawable.ic_media_pause : android.R.drawable.ic_media_play),
            isPlaying ? "Pause" : "Play", Player.COMMAND_PLAY_PAUSE);

        Bitmap genericAlbumArt = BitmapFactory.decodeResource(getResources(), android.R.drawable.ic_menu_gallery);

        Notification notification = new NotificationCompat.Builder(PlaybackService.this, CHANNEL_ID)
          .setContentTitle(currentTitle)
          .setContentText(currentArtist)
          .setSmallIcon(android.R.drawable.ic_media_play)
          .setLargeIcon(genericAlbumArt)
          .setPriority(NotificationCompat.PRIORITY_DEFAULT)
          .setVisibility(NotificationCompat.VISIBILITY_PUBLIC)
          .setOngoing(true)
          .addAction(prevAction)
          .addAction(playPauseAction)
          .addAction(nextAction)
          .setStyle(new MediaStyleNotificationHelper.MediaStyle(session)
              .setShowActionsInCompactView(0, 1, 2))
          .build();

        return new MediaNotification(NOTIFICATION_ID, notification);
          }

      @Override
      public boolean handleCustomCommand(MediaSession session, String action, android.os.Bundle extras) {
        return false;
      }
    });

    if (onReadyCallback != null) {
      new Handler(Looper.getMainLooper()).post(onReadyCallback);
      onReadyCallback = null;
    }
  }

  @Override
  public int onStartCommand(Intent intent, int flags, int startId) {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
      Bitmap genericAlbumArt = BitmapFactory.decodeResource(getResources(), android.R.drawable.ic_menu_gallery);
      Notification placeholderNotif = new NotificationCompat.Builder(this, CHANNEL_ID)
        .setContentTitle(currentTitle)
        .setContentText(currentArtist)
        .setSmallIcon(android.R.drawable.ic_media_play)
        .setLargeIcon(genericAlbumArt)
        .setPriority(NotificationCompat.PRIORITY_DEFAULT)
        .setVisibility(NotificationCompat.VISIBILITY_PUBLIC)
        .setOngoing(true)
        .setStyle(new MediaStyleNotificationHelper.MediaStyle(mediaSession))
        .build();
      startForeground(NOTIFICATION_ID, placeholderNotif);
    }
    return super.onStartCommand(intent, flags, startId);
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
