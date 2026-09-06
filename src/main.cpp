#include "./bindings/PyHelper.h"
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QLoggingCategory>
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

#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QJniObject>
#endif

namespace py = pybind11;
using namespace py::literals;

Q_LOGGING_CATEGORY(vermilion, "Vermilion")

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  app.setApplicationName("Vermilion");
  qRegisterMetaType<QMap<QByteArray, QByteArray>>("QMap<QByteArray,QByteArray>");

  auto res = PyHelper::init();
  if (auto result = PyHelper::init(); !result) {
    auto e = result.error();
    qCDebug(vermilion) << "Failed to initialize python.";
    qCDebug(vermilion) << "Error in: " << e.location.file_name() << " at " << e.location.function_name() << ":"
                       << e.location.line();
    qCDebug(vermilion) << e.debugContext;
    return -1;
  }

  try {
    py::initialize_interpreter();
  } catch (const py::error_already_set &e) {
    qCDebug(vermilion) << e.what();
    return -1;
  } catch (const std::exception &e) {
    qCDebug(vermilion) << "Failed to initialize python interpreter:" << e.what();
    return -1;
  } catch (...) {
    qCDebug(vermilion) << "Unknown fatal error during python initialization.";
    return -1;
  }

  py::gil_scoped_release release;
  QQmlApplicationEngine engine;
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); },
      Qt::QueuedConnection);

  engine.loadFromModule("Vermilion", "Main");
  int exitCode = app.exec();
  py::finalize_interpreter();
  return exitCode;
}
