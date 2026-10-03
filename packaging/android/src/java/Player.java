import io.github.x6carbon12.vermilion.PlaybackService;
import android.Manifest;
import android.content.pm.PackageManager;
import android.app.Activity;
import android.os.Build;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import androidx.media3.common.PlaybackException;
import androidx.media3.common.Player.Listener;
import androidx.media3.exoplayer.ExoPlayer;

public class Player {
  private ExoPlayer player;
  private Context context;
  private volatile long cachedPosition = 0;
  private volatile long cachedDuration = 0;

  private final Handler mainHandler = new Handler(Looper.getMainLooper());
  private final Runnable updateProgressAction = new Runnable() {
    @Override
    public void run() {
      if (player != null && player.isPlaying()) {
        cachedPosition = player.getCurrentPosition();
        long duration = player.getDuration();
        cachedDuration = duration < 0 ? 0 : duration;
        mainHandler.postDelayed(this, 200);
      }
    }
  };

  public Player(Context context) {
    this.context = context;

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
      if (ContextCompat.checkSelfPermission(context,
          Manifest.permission.POST_NOTIFICATIONS) != PackageManager.PERMISSION_GRANTED) {
        if (context instanceof Activity) {
          ActivityCompat.requestPermissions(
              (Activity) context,
              new String[] { Manifest.permission.POST_NOTIFICATIONS },
              101);
        }
      }
    }

    PlaybackService.start(context);
  }

  private ExoPlayer getPlayer() {
    if (player == null && PlaybackService.getInstance() != null) {
      player = PlaybackService.getInstance().getBasePlayer();
    }
    return player;
  }

  public void loadTrack(String url, String title, String artist, String artUrl) {
    mainHandler.post(() -> {
      PlaybackService service = PlaybackService.getInstance();
      if (service != null) {
        service.loadTrackInSession(url, title, artist, artUrl);
      }
    });
  }

  public void play() {
    mainHandler.post(() -> {
      if (getPlayer() != null) {
        player.play();
        mainHandler.post(updateProgressAction);
      }
    });
  }

  public void pause() {
    mainHandler.post(() -> {
      if (getPlayer() != null && player.isPlaying()) {
        player.pause();
        mainHandler.removeCallbacks(updateProgressAction);
      }
    });
  }

  public void seekTo(long positionMs) {
    cachedPosition = positionMs;
    mainHandler.post(() -> {
      if (getPlayer() != null) {
        player.seekTo(positionMs);
      }
    });
  }

  public void release() {
    mainHandler.post(() -> {
      mainHandler.removeCallbacks(updateProgressAction);
      player = null;
      context.stopService(new Intent(context, PlaybackService.class));
    });
  }

  public long getCurrentPosition() {
    return cachedPosition;
  }

  public long getDuration() {
    return cachedDuration;
  }
}
