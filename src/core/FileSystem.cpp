#include "FileSystem.h"
#include <expected>

namespace Core::FileSystem {

std::expected<void, Error> copyDirectoryRecursively(const QString &sourceDir, const QString &targetDir) {
  QDir srcDir(sourceDir);
  if (!srcDir.exists()) {
    return std::unexpected(Error{ErrorReason::NotFound, sourceDir, "Source directory doesn't exist."});
  }

  QDir tgtDir(targetDir);
  if (!tgtDir.exists()) {
    if (!tgtDir.mkpath(".")) {
      return std::unexpected(Error{ErrorReason::CreationFailed, targetDir, "Target directory could not be created."});
    }
  }

  const QDir::Filters allFilesFilter = QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System;
  const QFileInfoList entries = srcDir.entryInfoList(allFilesFilter);

  for (const QFileInfo &entryInfo : entries) {
    QString srcPath = entryInfo.absoluteFilePath();
    QString tgtPath = tgtDir.absoluteFilePath(entryInfo.fileName());

    if (entryInfo.isDir()) {
      auto result = copyDirectoryRecursively(srcPath, tgtPath);
      if (!result) {
        return std::unexpected(result.error());
      }
      continue;
    }

    if (QFile::exists(tgtPath)) {
      QFile::setPermissions(tgtPath, QFile::ReadOwner | QFile::WriteOwner);
      if (!QFile::remove(tgtPath)) {
        return std::unexpected(Error{ErrorReason::DeletionFailed, tgtPath, "The given file couldn't be deleted."});
      }
    }

    if (!QFile::copy(srcPath, tgtPath)) {
      return std::unexpected(Error{ErrorReason::CopyFailed, tgtPath, "Failed to copy a file to the given target."});
    }

    QFile::setPermissions(tgtPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser);
  }

  return {};
}
} // namespace Core::FileSystem
