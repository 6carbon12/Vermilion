import io.github.x6carbon12.vermilion.PlaybackService;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import androidx.core.content.ContextCompat;
import androidx.media3.common.MediaItem;
import androidx.media3.common.PlaybackException;
import androidx.media3.common.Player.Listener;
import androidx.media3.session.MediaController;
import androidx.media3.session.SessionToken;
import com.google.common.util.concurrent.ListenableFuture;

public class Player {
  private MediaController mediaController;
  private ListenableFuture<MediaController> controllerFuture;
  private Context context;

  private volatile long cachedPosition = 0;
  private volatile long cachedDuration = 0;

  private final Handler mainHandler = new Handler(Looper.getMainLooper());
  private final Runnable updateProgressAction = new Runnable() {
    @Override
    public void run() {
      if (mediaController != null) {
        boolean isPlaying = mediaController.isPlaying();
        int state = mediaController.getPlaybackState();

        if (isPlaying) {
          cachedPosition = mediaController.getCurrentPosition();
          long duration = mediaController.getDuration();
          cachedDuration = duration < 0 ? 0 : duration;
        }

        if (state != androidx.media3.common.Player.STATE_ENDED) {
          mainHandler.postDelayed(this, 200);
        }
      }
    }
  };

  public Player(Context context) {
    this.context = context;

    Intent intent = new Intent(context, PlaybackService.class);
    ContextCompat.startForegroundService(context, intent);

    SessionToken sessionToken =
      new SessionToken(context, new ComponentName(context, PlaybackService.class));

    controllerFuture = new MediaController.Builder(context, sessionToken).buildAsync();

    controllerFuture.addListener(() -> {
      try {
        mediaController = controllerFuture.get();

        mediaController.addListener(new Listener() {
          @Override
          public void onPlaybackStateChanged(int playbackState) {
            if (playbackState == androidx.media3.common.Player.STATE_ENDED) {
              safeOnTrackEnded();
            }
          }

          @Override
          public void onPlayerError(PlaybackException error) {
            if (error.getCause() instanceof java.io.EOFException ||
                error.errorCode == PlaybackException.ERROR_CODE_IO_UNSPECIFIED) {
              safeOnTrackEnded();
                }
          }
        });
      } catch (Exception e) {
        Log.e("Player", "Failed to connect to MediaSessionService", e);
      }
    }, ContextCompat.getMainExecutor(context)); 
  }

  private void safeOnTrackEnded() {
    try {
      onTrackEnded();
    } catch (UnsatisfiedLinkError e) {
      Log.w("Player", "Failed to call CPP native function 'onTrackEnded'");
    }
  }

  private native void onTrackEnded();

  public void loadTrack(String url, String title, String artist, String artUrl) {
    mainHandler.post(() -> {
      PlaybackService service = PlaybackService.getInstance();
      if (service != null) {
        // Routes through the service so the MediaSession registers the track and metadata
        service.loadTrackInSession(url, title, artist, artUrl);
      }
    });
  }

  public void play() {
    mainHandler.post(() -> {
      try {
        if (mediaController != null) {
          mediaController.play();
          mainHandler.post(updateProgressAction);
        }
      } catch (Exception e) {
        e.printStackTrace();
      }
    });
  }

  public void pause() {
    mainHandler.post(() -> {
      if (mediaController != null && mediaController.isPlaying()) {
        mediaController.pause();
        mainHandler.removeCallbacks(updateProgressAction);
      }
    });
  }

  public void seekTo(long positionMs) {
    cachedPosition = positionMs;
    mainHandler.post(() -> {
      if (mediaController != null) {
        mediaController.seekTo(positionMs);
      }
    });
  }

  public void release() {
    mainHandler.post(() -> {
      mainHandler.removeCallbacks(updateProgressAction);
      if (mediaController != null) {
        mediaController.release();
        mediaController = null;
      }
      if (controllerFuture != null) {
        MediaController.releaseFuture(controllerFuture);
        controllerFuture = null;
      }
    });
  }

  public long getCurrentPosition() { return cachedPosition; }

  public long getDuration() { return cachedDuration; }
}
