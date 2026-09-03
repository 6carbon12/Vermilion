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
#include "./bindings/PyHelper.h"
#include "src/bindings/YtDLPWorker.h"
#include <QTimer>

namespace py = pybind11;
using namespace py::literals;

Q_LOGGING_CATEGORY(vermilion, "Vermilion")

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  app.setApplicationName("Vermilion");

  auto res = PyHelper::init();
  if (auto result = PyHelper::init(); !result) {
    auto e = result.error();
    qCDebug(vermilion) << "Failed to initialize python.";
    qCDebug(vermilion) << "Error in: " << e.location.file_name() << " at " << e.location.function_name() << ":" << e.location.line();
    qCDebug(vermilion) << e.debugContext;
    return -1;
  }

  try {
    py::initialize_interpreter();
  } catch (const py::error_already_set& e) {
    qCDebug(vermilion) << e.what();
    return -1;
  } catch (const std::exception& e) {
    qCDebug(vermilion) << "Failed to initialize python interpreter:" << e.what();
    return -1;
  } catch (...) {
    qCDebug(vermilion) << "Unknown fatal error during python initialization.";
    return -1;
  }

  YtDLPWorker worker;

  QObject::connect(&worker, &YtDLPWorker::initFailed, [](PyHelper::Error e) {
      // Note: Adjust the 'e.message' field based on your exact PyHelper::Error struct definition
      qCritical() << "Initialization Failed."; 
  });

  QObject::connect(&worker, &YtDLPWorker::extractSuccess, [](const QString& url) {
      qInfo() << "Extraction Successful!";
      qInfo() << "Stream URL:" << url;
  });

  QObject::connect(&worker, &YtDLPWorker::extractFailed, [](const QString& errorMsg) {
      qCritical() << "Extraction Failed:" << errorMsg;
  });

  // 4. Run initialization synchronously
  qInfo() << "Initializing yt-dlp...";
  worker.init();

  QString testUrl = "https://www.youtube.com/watch?v=dQw4w9WgXcQ";

  QTimer::singleShot(0, [&worker, testUrl]() {
      qInfo() << "Starting extraction for:" << testUrl;
      worker.extractUrl(testUrl);
      });

  QQmlApplicationEngine engine;
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.loadFromModule("Vermilion", "Main");
  int exitCode = app.exec();
  py::finalize_interpreter();
  return exitCode;
}

