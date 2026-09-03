#pragma once

#include <QDir>
#include <expected>
#include <source_location>

namespace Core::FileSystem
{
using namespace Core::FileSystem;

enum class ErrorReason
{
  CreationFailed,
  DeletionFailed,
  CopyFailed,
  NotFound,
  Permission
};

struct Error
{
  ErrorReason reason;
  QString problematicPath;
  QString debugContext;
  std::source_location location = std::source_location::current();
};

std::expected<void, Error> copyDirectoryRecursively(const QString &sourceDir,
                                                    const QString &targetDir);
}; // namespace Core::FileSystem
