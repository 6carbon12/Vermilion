import android.app.Activity;
import android.content.Context;
import android.net.Uri;
import androidx.media3.common.MediaItem;
import androidx.media3.exoplayer.ExoPlayer;

public class Player {
  private ExoPlayer exoPlayer;
  private Context context;

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
        // TODO: Handle case: EOF can be reached if there is a network error and the file is not fully downloaded
        if (error.getCause() instanceof java.io.EOFException ||
            error.errorCode == androidx.media3.common.PlaybackException.ERROR_CODE_IO_UNSPECIFIED) {
          onTrackEnded();
        }
      }
    });
  }


  private native void onTrackEnded();

  public void setUrl(String url) {
    if (!(context instanceof Activity)) {
      return;
    }

    ((Activity) context).runOnUiThread(() -> {
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
    if (!(context instanceof Activity)) {
      return;
    }

    ((Activity) context).runOnUiThread(() -> {
      try {
        exoPlayer.play();
      } catch (Exception e) {
        e.printStackTrace();
      }
    });
  }

  public void pause() {
    ((Activity) context).runOnUiThread(() -> {
      if (exoPlayer != null && exoPlayer.isPlaying()) {
        exoPlayer.pause();
      }
    });
  }

  public void resume() {
    ((Activity) context).runOnUiThread(() -> {
      if (exoPlayer != null && !exoPlayer.isPlaying()) {
        exoPlayer.play();
      }
    });
  }

  public void release() {
    ((Activity) context).runOnUiThread(() -> {
      if (exoPlayer != null) {
        exoPlayer.release();
        exoPlayer = null;
      }
    });
  }
}
