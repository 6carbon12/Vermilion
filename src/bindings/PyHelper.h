#pragma once

#include <QString>
#include <expected>
#include <source_location>

namespace PyHelper {

enum class ErrorReason { ModuleNotFound, FileError, PyNotInit, ImportFailed };

struct Error {
  ErrorReason reason;
  QString debugContext;
  std::source_location location = std::source_location::current();
};

std::expected<void, Error> init();
std::expected<void, Error> installModule(const QString &modulePath, const QString &moduleName = "");
} // namespace PyHelper
