#include "PyHelper.h"
#include "../core/FileSystem.h"
#include <QDir>
#include <QStandardPaths>
#include <expected>
#include <qloggingcategory.h>
#include <qtenvironmentvariables.h>

namespace PyHelper {

Q_LOGGING_CATEGORY(PyHelper, "PyHelper")

std::expected<void, Error> init() {
#ifdef Q_OS_LINUX
  if (qEnvironmentVariableIsEmpty("PYTHONHOME")) {
    qCWarning(PyHelper) << "PYTHONHOME not set.";
    return std::unexpected(Error({ErrorReason::FileError, "PYTHONHOME must be set before launching the program."}));
  }

  const QString PYTHONHOME = qgetenv("PYTHONHOME");

  if (!QDir().exists(PYTHONHOME)) {
    qCWarning(PyHelper) << "PYTHONHOME is not valid.";
    return std::unexpected(Error({ErrorReason::FileError, "Given PYTHONHOME path doesn't exist. PYTHONHOME=" + PYTHONHOME}));
  }

  qCDebug(PyHelper) << "Python initalized.";
  return {};
#endif
  QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QString stdLibDir = dataDir + "/python/lib/python3.11";

  if (QDir().exists(stdLibDir)) {
    qCDebug(PyHelper) << "Standard library exists, setting 'PYTHONHOME' and returning.";
    qputenv("PYTHONHOME", (dataDir + "/python").toUtf8());
    return {};
  }

  QDir().mkpath(stdLibDir);
  auto result = Core::FileSystem::copyDirectoryRecursively("assets:/python311", stdLibDir);

  if (result) {
    qCDebug(PyHelper) << "Initalized Python.";
    qputenv("PYTHONHOME", (dataDir + "/python").toUtf8());
    return {};
  }

  auto e = result.error();
  ErrorReason errReason;
  QString errStr;

  switch (e.reason) {
    namespace FS = Core::FileSystem;
  case FS::ErrorReason::NotFound:
    errReason = ErrorReason::ModuleNotFound;
    errStr = "Standard library not found.";
    break;
  default:
    errReason = ErrorReason::FileError;
    errStr = e.problematicPath + " : " + e.debugContext;
    break;
  }

  qCWarning(PyHelper) << "Initalization failed:";
  qCWarning(PyHelper) << errStr;
  return std::unexpected(Error({errReason, errStr}));
}

std::expected<void, Error> installModule(const QString &modulePath, const QString &moduleName) {
  if (qEnvironmentVariableIsEmpty("PYTHONHOME")) {
    qCWarning(PyHelper) << "Cannot install module at:" << modulePath << "because python is not initalized";
    return std::unexpected(Error({ErrorReason::PyNotInit, "Initalize python before trying to install modules."}));
  }

  const QString PYTHONHOME = qgetenv("PYTHONHOME");
  QString sitePkgs = PYTHONHOME + "/lib/python3.11/site-packages";
  QDir().mkpath(sitePkgs);
  QString moduleTgtPath = sitePkgs + "/" + (moduleName.isEmpty() ? modulePath.section('/', -1) : moduleName);

#ifdef Q_OS_LINUX
  if (!QDir(moduleTgtPath).exists() || QDir(moduleTgtPath).isEmpty()) {
    qCDebug(PyHelper) << "Module does not exit. Bad things might happen.";
    return std::unexpected(Error({ErrorReason::FileError, "Module not installed. Package might not be installed correctly."}));
  }

  return {};
#endif

  if (QDir(moduleTgtPath).exists() && !QDir(moduleTgtPath).isEmpty()) {
    qCDebug(PyHelper) << "Module already exists, skipping installation:" << moduleTgtPath;
    return {};
  }

  auto result = Core::FileSystem::copyDirectoryRecursively(modulePath, moduleTgtPath);

  if (result) {
    qCDebug(PyHelper) << "Installed module:" << modulePath;
    return {};
  }

  auto e = result.error();
  ErrorReason errReason;
  QString errStr;

  switch (e.reason) {
    namespace FS = Core::FileSystem;
  case FS::ErrorReason::NotFound:
    errStr = "Module: " + modulePath + " couldn't be found";
    errReason = ErrorReason::ModuleNotFound;
    break;
  default:
    errStr = e.problematicPath + " : " + e.debugContext;
    errReason = ErrorReason::FileError;
    break;
  }

  qCDebug(PyHelper) << "Failed to install module" << modulePath;
  qCDebug(PyHelper) << errStr;
  return std::unexpected(Error({errReason, errStr}));
}
} // namespace PyHelper
