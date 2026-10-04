#include "Player.h"
#include "AndroidPlayer.h"
#include <memory>
#include <qlogging.h>

namespace Core {
Core::Player *Core::Player::self = nullptr;

std::unique_ptr<Player> Player::create() {
  if (self != nullptr) {
    qCritical() << "Player: Refusing to create duplicate instance of player.";
    return nullptr;
  }
#ifdef Q_OS_ANDROID
  std::unique_ptr<AndroidPlayer> u_ptr = std::make_unique<AndroidPlayer>();
  self = u_ptr.get();
  return u_ptr;
#else
  std::unique_ptr<DesktopPlayer> u_ptr = std::make_unique<DesktopPlayer>();
  self = u_ptr.get();
  return u_ptr;
#endif
}

Player::~Player() {
  if (self == this) {
    self = nullptr;
  }
}

Player *Player::instance() { return self; }
} // namespace Core
