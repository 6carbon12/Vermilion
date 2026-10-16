cmake_minimum_required(VERSION 3.16)
project(Vermilion VERSION 0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

set(PYBIND11_NOPYTHON ON CACHE BOOL "Disable pybind11 python discovery")

FetchContent_Declare(
  pybind11
  URL https://github.com/pybind/pybind11/archive/refs/tags/v3.1.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(pybind11)

find_package(Qt6 6.12 REQUIRED COMPONENTS Core Qml Quick QuickControls2)
qt_standard_project_setup(REQUIRES 6.12)

set(APP_NAME vermilion)
set(ANDROID_TARGET ${APP_NAME}_android)
set(LINUX_TARGET ${APP_NAME}_linux)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CPP_SOURCES
  src/main.cpp
  src/bindings/PyHelper.cpp
  src/bindings/PyHelper.h
  src/bindings/YtWorker.cpp
  src/bindings/YtWorker.h
  src/core/FileSystem.cpp
  src/core/FileSystem.h
  src/core/Network.cpp
  src/core/Network.h
  src/core/audio/Player.cpp
  src/core/audio/Player.h
  src/managers/PlayerEngine.h
  src/managers/PlayerEngine.cpp
  src/bindings/YtEngine.h
  src/bindings/YtEngine.cpp
  src/core/audio/PlayerState.h
  src/core/audio/Track.h
)

set(CPP_ANDROID_SOURCES
  src/core/audio/AndroidPlayer.cpp
  src/core/audio/AndroidPlayer.h
)

set(CPP_LINUX_SOURCES
  src/core/audio/DesktopPlayer.cpp
  src/core/audio/DesktopPlayer.h
  src/managers/MprisController.h
  src/managers/MprisController.cpp
)

set(CPP_INCLUDES
  src/core
  src/core/audio
  src/bindings
  src/managers
  src/models
  src/dev
)

set(QML_FILES
  src/qml/Main.qml
  src/qml/MainShell.qml
  src/qml/pages/Home.qml
  src/qml/pages/Search.qml
  src/qml/pages/Library.qml
  src/qml/pages/Settings.qml
  src/qml/pages/SearchResults.qml
  src/qml/widgets/BottomBar.qml
  src/qml/widgets/SideBar.qml
  src/qml/components/IconButton.qml
  src/qml/components/BasePage.qml
  src/qml/components/SearchBar.qml
)

set(QML_SOURCES
  src/managers/YtController.h
  src/managers/YtController.cpp
  src/managers/PlayerController.h
  src/managers/PlayerController.cpp
  src/core/audio/PlayerState.h
  src/core/audio/Track.h
  src/models/TrackListModel.cpp
  src/models/TrackListModel.h
  src/dev/HotReloader.h
  src/dev/HotReloader.cpp
)

file(
  GLOB_RECURSE
  APP_ICONS
  RELATIVE ${CMAKE_SOURCE_DIR}
  CONFIGURE_DEPENDS "assets/icons/*.svg"
)

if(ANDROID)
  qt_add_executable(${ANDROID_TARGET} ${CPP_SOURCES} ${CPP_ANDROID_SOURCES})
  set_target_properties(${ANDROID_TARGET} PROPERTIES
    OUTPUT_NAME "vermilion"
  )

  # `slots` is a macro used by QT
  # but `slots` is used as a variable in Python C API
  # So we disable that keyword with this:
  target_compile_definitions(${ANDROID_TARGET} PRIVATE QT_NO_KEYWORDS)
  target_include_directories(${ANDROID_TARGET} PRIVATE ${CPP_INCLUDES})

  qt_add_qml_module(
    ${ANDROID_TARGET}
    URI ${PROJECT_NAME}
    VERSION 1.0
    QML_FILES
    ${QML_FILES}
    SOURCES
    ${QML_SOURCES}
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Vermilion
    RESOURCES
    ${APP_ICONS}
  )

  FetchContent_Declare(
    custom_android_libs
    URL "https://github.com/6carbon12/Vermilion/releases/download/\
v1.0/vermilion-extra-libs.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(custom_android_libs)

  set(ANDROID_PKG_DIR "${CMAKE_SOURCE_DIR}/packaging/android")
  target_include_directories(
    ${ANDROID_TARGET}
    PRIVATE "${ANDROID_PKG_DIR}/include"
  )
  target_link_libraries(${ANDROID_TARGET}
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Network
    PRIVATE pybind11::embed
    PRIVATE Qt6::QuickControls2
    PRIVATE "${ANDROID_PKG_DIR}/libs/arm64-v8a/libpython3.11.so"
    PRIVATE "${ANDROID_PKG_DIR}/libs/arm64-v8a/libffi.so"
    PRIVATE "${ANDROID_PKG_DIR}/libs/arm64-v8a/libcrypto_3.so"
    PRIVATE "${ANDROID_PKG_DIR}/libs/arm64-v8a/libssl_3.so"
    PRIVATE "${ANDROID_PKG_DIR}/libs/arm64-v8a/libsqlite3.so"
  )

  set_property(
    TARGET ${ANDROID_TARGET}
    PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR ${ANDROID_PKG_DIR}
  )
  set_property(
    TARGET ${ANDROID_TARGET}
    PROPERTY QT_ANDROID_APP_NAME "Vermilion"
  )
  set_property(TARGET ${ANDROID_TARGET}
    PROPERTY QT_ANDROID_PACKAGE_NAME "io.github.x6carbon12.vermilion"
  )

  set_property(TARGET ${ANDROID_TARGET} PROPERTY
    QT_ANDROID_EXTRA_LIBS
    "${custom_android_libs_SOURCE_DIR}/libqjs.so"
  )
else()
  find_package(Qt6 6.12 COMPONENTS Core DBus REQUIRED)

  qt_add_dbus_adaptor(generated_mpris_sources
    "src/dbus/org.mpris.MediaPlayer2.xml"
    "src/managers/MprisController.h"
    "MprisController"
    "mpris2_adaptor"
    "Mpris2Adaptor"
  )

  qt_add_dbus_adaptor(generated_player_sources
    "src/dbus/org.mpris.MediaPlayer2.Player.xml"
    "src/managers/MprisController.h"
    "MprisController"
    "player_adaptor"
    "PlayerAdaptor"
  )

  set(CPP_SOURCES_DEBUG
    src/dev/HotReloader.h
    src/dev/HotReloader.cpp
  )
  qt_add_executable(${LINUX_TARGET}
    ${CPP_SOURCES}
    ${CPP_SOURCES_DEBUG}
    ${CPP_LINUX_SOURCES}
    ${generated_mpris_sources}
    ${generated_player_sources}
  )
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${LINUX_TARGET} PRIVATE
      HOTRELOAD_QML_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src/qml"
    )
  endif()
  set_target_properties(${LINUX_TARGET} PROPERTIES OUTPUT_NAME "vermilion-core")
  qt_add_qml_module(
    ${LINUX_TARGET}
    URI ${PROJECT_NAME}
    VERSION 1.0
    QML_FILES
    ${QML_FILES}
    SOURCES
    ${QML_SOURCES}
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Vermilion
    RESOURCES
    ${APP_ICONS}
  )

  target_compile_definitions(${LINUX_TARGET} PRIVATE QT_NO_KEYWORDS)
  target_include_directories(${LINUX_TARGET} PRIVATE
    ${CPP_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
  )

  # All Python stuff is supplied via command line.
  if(STANDALONE_PYTHON_DIR)
    set(Python3_ROOT_DIR "${STANDALONE_PYTHON_DIR}")
    set(Python3_FIND_STRATEGY LOCATION)
    set(Python3_FIND_REGISTRY NEVER)
    set(Python3_FIND_FRAMEWORK NEVER)
  endif()

  find_package(Python3 3.11 EXACT COMPONENTS Development.Embed REQUIRED)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(MPV REQUIRED IMPORTED_TARGET mpv)
  pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)

  target_link_options(${LINUX_TARGET} PRIVATE "-Wl,--trace")
  target_link_libraries(${LINUX_TARGET} PRIVATE
    PRIVATE PkgConfig::AVUTIL
    PRIVATE Qt6::Quick
    PRIVATE Qt6::DBus
    PRIVATE Qt6::QuickControls2
    PRIVATE pybind11::embed
    PRIVATE PkgConfig::MPV
    PRIVATE Python3::Python
  )
endif()
