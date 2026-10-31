#pragma once

#include <QMetaObject>
#include <QMetaType>
#include <QString>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace Core {

struct Artist {
  Q_GADGET
  QML_ELEMENT

  Q_PROPERTY(QString id MEMBER id)
  Q_PROPERTY(QString name MEMBER name)

public:
  QString name;
  QString id;
};

} // namespace Core

Q_DECLARE_METATYPE(Core::Artist)
