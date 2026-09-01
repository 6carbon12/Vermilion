#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QProcessEnvironment>
#include <QQmlApplicationEngine>
#include <QStandardPaths>
#include <QtLogging>
#include <exception>
#include <pybind11/detail/descr.h>
#include <pybind11/embed.h>
#include <pybind11/eval.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <qdir.h>
#include <qlogging.h>
#include <qloggingcategory.h>
#include <qtenvironmentvariables.h>
#include <QLoggingCategory>
#include <string>
#include <vector>

namespace py = pybind11;
using namespace py::literals;

Q_LOGGING_CATEGORY(vermilion, "Vermilion")

// Copies all files/folders within sourceDir into targetDir recursively
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

// Setups python standard library
bool initPython() {
  QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QString stdLibDir = dataDir + "/lib/python3.11";
  if (!QDir().exists(stdLibDir)) {
    QDir().mkpath(stdLibDir);
    if (!copyDirectoryRecursively("assets:/python311", stdLibDir))
      return false;
  }
  qputenv("PYTHONHOME", dataDir.toStdString());
  return true;
}

// Intalls a module at modulePath as moduleName, so that python can find it
bool installModule(QString modulePath, QString moduleName) {
  const QString PYTHONHOME = qgetenv("PYTHONHOME");
  if (PYTHONHOME.isEmpty()) {
    qCDebug(vermilion) << "PYTHONHOME not set properly";
    return false;
  }
  QString sitePkgs = PYTHONHOME + "/lib/python3.11/site-packages";
  QDir().mkpath(sitePkgs);
  if (QDir().exists(sitePkgs + "/" + moduleName)) {
    qCDebug(vermilion) << "Skipping install for module: " << moduleName << ": Module already installed";
    return true;
  }

  if (!copyDirectoryRecursively(modulePath, sitePkgs + "/" + moduleName))
    return false;
  return true;
}

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  app.setApplicationName("Vermilion");

  if (!initPython()) {
    qCDebug(vermilion) << "Python init failed";
    return -1;
  }

  if (
      !installModule("assets:/yt_dlp", "yt_dlp") || 
      !installModule("assets:/yt_dlp_ejs", "yt_dlp_ejs") ||
      !installModule("assets:/certifi", "certifi")
     )
  {
    qCDebug(vermilion) << "Failed to install yt-dlp/certifi modules";
    return -1;
  }

  try {
    py::initialize_interpreter();
  } catch (...) {
    qCDebug(vermilion) << "Failed to intialize python interpreter.";
  }

  try {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString nativeLibDir = QCoreApplication::applicationDirPath();
    py::module_ sys = py::module_::import("sys");
    sys.attr("path").attr("append")(nativeLibDir.toStdString());
    py::str temp = ":";
    std::string path = temp.attr("join")(sys.attr("path")).cast<std::string>();
    qCDebug(vermilion) << "path: " << path;

    qCDebug(vermilion) << "Importing yt_dlp...";
    py::module_ yt_dlp = py::module_::import("yt_dlp");
    qCDebug(vermilion) << "Imported yt_dlp.";
    qCDebug(vermilion) << "Getting YoutubeDL...";
    py::object YoutubeDL = yt_dlp.attr("YoutubeDL");
    qCDebug(vermilion) << "Got YoutubeDL.";

    // Locate the native library directory where Android extracted our binary
    QString jsBinaryPath = nativeLibDir + "/libqjs.so";

    py::dict runtime_config;
    // Convert QString to std::string for pybind11
    runtime_config["path"] = jsBinaryPath.toStdString(); 

    py::dict js_runtimes;
    js_runtimes["quickjs"] = runtime_config; 

    py::dict ydl_opts;
    ydl_opts["format"] = "bestaudio/best";
    ydl_opts["quiet"] = true;
    ydl_opts["noplaylist"] = true; // Prevent downloading entire playlists
    ydl_opts["js_runtimes"] = js_runtimes;
    ydl_opts["download"] = false;

    qCDebug(vermilion) << "Creating ydl...";
    py::object ydl = YoutubeDL(ydl_opts);

    std::string targetUrl = "https://www.youtube.com/watch?v=YuWUJg3Kdks";

    // Extract info
    qCDebug(vermilion) << "Extracting URL: " << targetUrl;
    py::object info = ydl.attr("extract_info")(targetUrl, "download"_a = false);

    if (info.contains("url") && !info["url"].is_none()) {
      std::string audioURL = info["url"].cast<std::string>();
      qCDebug(vermilion) << "Extracted URL: " << audioURL; 
    } else {
      qCDebug(vermilion) << "Extraction failed or direct 'url' key is missing.";
    }
  } catch (py::error_already_set e) {
    qCDebug(vermilion) << "Python error: " << e.what();
    py::finalize_interpreter();
    return -1;
  } catch(std::exception e) {
    qCDebug(vermilion) << "CPP error: " << e.what();
    py::finalize_interpreter();
    return -1;
  } catch (...) {
    qCDebug(vermilion) << "Unknown error occured while extracting URL.";
  }

  QQmlApplicationEngine engine;
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.loadFromModule("Vermilion", "Main");
  py::finalize_interpreter();
  return app.exec();
}

