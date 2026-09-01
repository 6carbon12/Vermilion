#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QProcessEnvironment>
#include <QQmlApplicationEngine>
#include <QStandardPaths>
#include <QtLogging>
#include <pybind11/embed.h>
#include <pybind11/eval.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <qlogging.h>
#include <qloggingcategory.h>
#include <qtenvironmentvariables.h>
#include <QLoggingCategory>

namespace py = pybind11;
using namespace py::literals;

Q_LOGGING_CATEGORY(vermilion, "Vermilion")

bool copyDirectoryRecursively(const QString &sourceDir,
                              const QString &targetDir) {
  QDir srcDir(sourceDir);
  if (!srcDir.exists())
    return false;

  QDir tgtDir(targetDir);
  if (!tgtDir.exists())
    if (!tgtDir.mkpath("."))
      return false;


  const QDir::Filters allFilesFilter = QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System;
  const QFileInfoList entries = srcDir.entryInfoList(allFilesFilter);

  for (const QFileInfo &entryInfo : entries) {
    QString srcPath = entryInfo.absoluteFilePath();
    QString tgtPath = tgtDir.absoluteFilePath(entryInfo.fileName());

    if (entryInfo.isDir()) {
      // Recursively dive into subdirectories
      if (!copyDirectoryRecursively(srcPath, tgtPath))
        return false;
    } else {
      if (QFile::exists(tgtPath))
        QFile::remove(tgtPath);

      // Copy individual file
      if (!QFile::copy(srcPath, tgtPath))
        return false;
    }
  }

  return true;
}

bool initPython() {
  QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QString stdLibDir = dataDir + "/lib/python3.11";
  if (!QDir().exists(stdLibDir)) {
    QDir().mkpath(stdLibDir);
    if (!copyDirectoryRecursively("assets:/python311/", stdLibDir))
      return false;
  }
  qputenv("PYTHONHOME", dataDir.toStdString());
  return true;
}

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  app.setApplicationName("Vermilion");

  if (!initPython()) {
    qDebug() << "Python init failed";
    return -1;
  }

  try {
    py::scoped_interpreter guard{};
  } catch (py::error_already_set &e) {
    qCDebug(vermilion) << "Failed to start python interpreter";
    qCDebug(vermilion) << "Python error:" << e.what();
  }

  QQmlApplicationEngine engine;
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.loadFromModule("Vermilion", "Main");
  return app.exec();
}

