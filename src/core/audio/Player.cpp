#include "Player.h"
#include "AndroidPlayer.h"
#include <memory>

namespace Core {
  std::unique_ptr<Player> Player::create() {
#ifdef Q_OS_ANDROID
    return std::make_unique<AndroidPlayer>();
#else
    return std::make_unique<DesktopPlayer>();
#endif
  }
}
