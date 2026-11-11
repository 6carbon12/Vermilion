#pragma once

#include <qobject.h>
#include <qqmlintegration.h>

namespace Core {
class PlayerState : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("PlayerState is an Enum Container.")
public:
  enum class State {
    Initialized = 0, /// Player is just newly created, and has no track in it.
    Buffering = 1,   /// Player is performing some network request and isn't playing anything.
    Playing = 2,     /// Player is actively playing a track, which user can listen.
    Paused = 3,      /// User has paused the track.
    Error = 4        /// Some error occured within player.
  };
  Q_ENUM(State);
};
} // namespace Core
