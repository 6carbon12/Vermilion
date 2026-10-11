#include "PyHelper.h"
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
#include "HotReloader.h"

namespace py = pybind11;
using namespace py::literals;

Q_LOGGING_CATEGORY(vermilion, "Vermilion")

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  app.setApplicationName("Vermilion");
  qRegisterMetaType<QMap<QByteArray, QByteArray>>();

  if (auto result = PyHelper::init(); !result) {
    auto e = result.error();
    qCFatal(vermilion) << "Failed to initialize python.";
    qCFatal(vermilion) << "Error in: " << e.location.file_name() << " at " << e.location.function_name() << ":"
                       << e.location.line();
    qCFatal(vermilion) << e.debugContext;
    return -1;
  }

  try {
    py::initialize_interpreter();
  } catch (const py::error_already_set &e) {
    qCFatal(vermilion) << e.what();
    return -1;
  } catch (const std::exception &e) {
    qCFatal(vermilion) << "Failed to initialize python interpreter:" << e.what();
    return -1;
  } catch (...) {
    qCFatal(vermilion) << "Unknown fatal error during python initialization.";
    return -1;
  }

  int exitCode = 0;

  // Create a new scope block for the engine and the `GIL` release
  {
    py::gil_scoped_release release;
    QQmlApplicationEngine engine;

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    QString qmlSourceDir = "";
#ifdef QT_DEBUG
#define XSTR(x) STR(x)
#define STR(x) #x
    qmlSourceDir = QString::fromUtf8(XSTR(HOTRELOAD_QML_DIR)).remove('"');
#endif

    HotReloader hotReloader(&engine, qmlSourceDir);
    qmlRegisterSingletonInstance("Vermilion.Dev", 1, 0, "HotReloader", &hotReloader);

#ifdef QT_DEBUG
    qDebug() << "Running Debug build.";
    qDebug() << "Loading local file Main.qml at:" << qmlSourceDir;
    engine.load(QUrl::fromLocalFile(qmlSourceDir + "/Main.qml"));
#else
    qDebug() << "Running Release build.";
    engine.loadFromModule("Vermilion", "Main");
#endif

    exitCode = app.exec();
  }

  py::finalize_interpreter();

  return exitCode;
}
