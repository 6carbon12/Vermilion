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
  }

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
    if (exoPlayer != null) {
      exoPlayer.release();
      exoPlayer = null;
    }
  }
}
