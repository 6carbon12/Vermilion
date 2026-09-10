#pragma once

#include <qobject.h>
#include <qqmlintegration.h>

namespace Core {
class PlayerState : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("PlayerState is an Enum Container.")
public:
  enum class State { Initialized = 0, Playing = 1, Paused = 2, Error = 3 };
  Q_ENUM(State);
};
} // namespace Core
