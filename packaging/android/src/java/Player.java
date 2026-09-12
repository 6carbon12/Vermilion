import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.content.Context;
import android.net.Uri;
import androidx.media3.common.MediaItem;
import androidx.media3.exoplayer.ExoPlayer;

public class Player {
  private ExoPlayer exoPlayer;
  private Context context;

  private volatile long cachedPosition = 0;
  private volatile long cachedDuration = 0;

  private final Handler mainHandler = new Handler(Looper.getMainLooper());

  private final Runnable updateProgressAction = new Runnable() {
    @Override
    public void run() {
      if (exoPlayer != null) {
        boolean isPlaying = exoPlayer.isPlaying();
        int state = exoPlayer.getPlaybackState();

        if (isPlaying) {
          cachedPosition = exoPlayer.getCurrentPosition();
          long duration = exoPlayer.getDuration();
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
    this.exoPlayer = new ExoPlayer.Builder(context).build();

    this.exoPlayer.addListener(new androidx.media3.common.Player.Listener() {
      @Override
      public void onPlaybackStateChanged(int playbackState) {
        if (playbackState == androidx.media3.common.Player.STATE_ENDED) {
          onTrackEnded();
        }
      }

      @Override
      public void onPlayerError(androidx.media3.common.PlaybackException error) {
        if (error.getCause() instanceof java.io.EOFException ||
            error.errorCode == androidx.media3.common.PlaybackException.ERROR_CODE_IO_UNSPECIFIED) {
          onTrackEnded();
            }
      }
    });
  }

  private native void onTrackEnded();

  public void setUrl(String url) {
    mainHandler.post(() -> {
      try {
        Uri mediaUri = Uri.parse(url);
        exoPlayer.setMediaItem(MediaItem.fromUri(mediaUri));
        exoPlayer.prepare();
      } catch (Exception e) {
        e.printStackTrace();
      }
    });
  }

  public void play() {
    mainHandler.post(() -> {
      try {
        if (exoPlayer != null) {
          exoPlayer.play();
          mainHandler.post(updateProgressAction);
        }
      } catch (Exception e) {
        e.printStackTrace();
      }
    });
  }

  public void pause() {
    mainHandler.post(() -> {
      if (exoPlayer != null && exoPlayer.isPlaying()) {
        exoPlayer.pause();
        mainHandler.removeCallbacks(updateProgressAction);
      }
    });
  }

  public void seekTo(long positionMs) {
    cachedPosition = positionMs;
    mainHandler.post(() -> {
      if (exoPlayer != null) {
        exoPlayer.seekTo(positionMs);
      }
    });
  }

  public void release() {
    mainHandler.post(() -> {
      if (exoPlayer != null) {
        mainHandler.removeCallbacks(updateProgressAction);
        exoPlayer.release();
        exoPlayer = null;
      }
    });
  }

  public long getCurrentPosition() {
    return cachedPosition;
  }

  public long getDuration() {
    return cachedDuration;
  }
}
