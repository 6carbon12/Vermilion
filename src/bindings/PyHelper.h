#pragma once

#include <QString>
#include <expected>
#include <source_location>

namespace PyHelper {

enum class ErrorReason { ModuleNotFound, FileError, PyNotInit, ImportFailed };

struct Error {
  ErrorReason reason;
  QString debugContext; ///< Human readable error message.
  std::source_location location = std::source_location::current();
};

/// @brief Initalizes Environment.
///
/// Setups standard python runtime, installs python standard library
/// @note Does not start the python interpreter.
std::expected<void, Error> init();

/// @brief Installs module at `modulePath` as `moduleName`
/// @param modulePath Path to the module folder. Example `~/yt-dlp`, `/python/modules/setuptools`
/// @param moduleName Name of the module. Can be anything, but this is the name that must be used when importing.
/// Defaults to name of the folder if left empty. Example `~/mouleName` becomes available as `moduleName`.
std::expected<void, Error> installModule(const QString &modulePath, const QString &moduleName = "");
} // namespace PyHelper
