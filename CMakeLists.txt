cmake_minimum_required(VERSION 3.16)
project(Vermilion VERSION 0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

if(ANDROID)
  set(PYBIND11_NOPYTHON ON CACHE BOOL "Disable pybind11 python discovery")
endif()

FetchContent_Declare(
  pybind11
  URL https://github.com/pybind/pybind11/archive/refs/tags/v2.12.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(pybind11)

find_package(Qt6 6.11 REQUIRED COMPONENTS Core Qml Quick Multimedia)
qt_standard_project_setup(REQUIRES 6.11)

set(APP_NAME vermilion)
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
  src/core/audio/AndroidPlayer.cpp
  src/core/audio/AndroidPlayer.h
  src/managers/PlayerEngine.h
  src/managers/PlayerEngine.cpp
  src/bindings/YtEngine.h
  src/bindings/YtEngine.cpp
)

set(CPP_INCLUDES
  src/core
  src/core/audio
  src/bindings
  src/managers
)

qt_add_executable(${APP_NAME} ${CPP_SOURCES})

qt_add_qml_module(
  ${APP_NAME}
  URI ${PROJECT_NAME}
  VERSION 1.0
  QML_FILES
  src/qml/Main.qml
  SOURCES
  src/managers/YtController.h
  src/managers/YtController.cpp
  src/managers/PlayerController.h
  src/managers/PlayerController.cpp
  src/core/audio/PlayerState.h
  src/core/audio/Track.h
)

# `slots` is a macro used by QT
# but `slots` is used as a variable in Python C API
# So we disable that keyword with this:
target_compile_definitions(${APP_NAME} PRIVATE QT_NO_KEYWORDS)

target_include_directories(${APP_NAME} PRIVATE ${CPP_INCLUDES})

if(ANDROID)
  set(P4A_DIR "$ENV{HOME}/.local/share/python-for-android/build")
  set(P4A_PYTHON_BUILD "${P4A_DIR}/other_builds/python3/arm64-v8a__ndk_target_27/python3")
  set(P4A_LIBS_COLLECTION "${P4A_DIR}/libs_collections/embed/arm64-v8a")

  FetchContent_Declare(
    custom_android_libs
    URL "https://github.com/6carbon12/Vermilion/releases/download/v1.0/vermilion-extra-libs.tar.gz"
  )
  FetchContent_MakeAvailable(custom_android_libs)

  set(ANDROID_PKG_STAGE "${CMAKE_CURRENT_BINARY_DIR}/android_pkg_stage")
  file(REMOVE_RECURSE "${ANDROID_PKG_STAGE}")

  file(GLOB_RECURSE ALL_JAVA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/packaging/android/src/*.java")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${ALL_JAVA_FILES})

  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/packaging/android/src"
    DESTINATION "${ANDROID_PKG_STAGE}"
  )
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/packaging/android/AndroidManifest.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/packaging/android/build.gradle"
    DESTINATION "${ANDROID_PKG_STAGE}"
  )

  file(MAKE_DIRECTORY "${ANDROID_PKG_STAGE}/assets")
  file(MAKE_DIRECTORY "${ANDROID_PKG_STAGE}/libs/arm64-v8a")

  file(CREATE_LINK
    "${P4A_PYTHON_BUILD}/android-build/android-root/lib/python3.11"
    "${ANDROID_PKG_STAGE}/assets/python311"
    SYMBOLIC
  )

  file(GLOB P4A_PACKAGES "${P4A_DIR}/python-installs/embed/arm64-v8a/*")
  foreach(pkg_path ${P4A_PACKAGES})
    get_filename_component(pkg_name "${pkg_path}" NAME)
    file(CREATE_LINK "${pkg_path}" "${ANDROID_PKG_STAGE}/assets/${pkg_name}" SYMBOLIC)
  endforeach()

  file(GLOB DYNLOAD_SO_FILES "${P4A_PYTHON_BUILD}/android-build/android-root/lib/python3.11/lib-dynload/*.so")
  foreach(so_file ${DYNLOAD_SO_FILES})
    get_filename_component(so_name "${so_file}" NAME)
    file(CREATE_LINK "${so_file}" "${ANDROID_PKG_STAGE}/libs/arm64-v8a/${so_name}" SYMBOLIC)
  endforeach()

  set(PYTHON_INC_DIR "${P4A_PYTHON_BUILD}/Include")
  set(PYTHON_SO "${P4A_PYTHON_BUILD}/android-build/libpython3.11.so")

  target_include_directories(${APP_NAME} PRIVATE "${PYTHON_INC_DIR}")

  target_link_libraries(${APP_NAME}
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Multimedia
    PRIVATE Qt6::Network
    PRIVATE pybind11::embed
    PRIVATE "${PYTHON_SO}"
    PRIVATE "${P4A_LIBS_COLLECTION}/libffi.so"
    PRIVATE "${P4A_LIBS_COLLECTION}/libcrypto.so"
    PRIVATE "${P4A_LIBS_COLLECTION}/libssl.so"
    PRIVATE "${P4A_LIBS_COLLECTION}/libsqlite3.so"
  )

  set_property(TARGET ${APP_NAME} PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR "${ANDROID_PKG_STAGE}")
  set_property(TARGET ${APP_NAME} PROPERTY QT_ANDROID_APP_NAME "Vermilion")
  set_property(TARGET ${APP_NAME} PROPERTY QT_ANDROID_PACKAGE_NAME "io.github.x6carbon12.vermilion")

  set_property(TARGET ${APP_NAME} PROPERTY
    QT_ANDROID_EXTRA_LIBS
    "${PYTHON_SO}"
    "${P4A_LIBS_COLLECTION}/libffi.so"
    "${P4A_LIBS_COLLECTION}/libcrypto.so"
    "${P4A_LIBS_COLLECTION}/libssl.so"
    "${P4A_LIBS_COLLECTION}/libsqlite3.so"
    "${custom_android_libs_SOURCE_DIR}/libqjs.so"
    "${custom_android_libs_SOURCE_DIR}/libcrypto_3.so"
    "${custom_android_libs_SOURCE_DIR}/libssl_3.so"
  )
else()
  find_package(Python3 COMPONENTS Development.Embed REQUIRED)
  target_link_libraries(vermilion PRIVATE
    Qt6::Quick
    PRIVATE Qt6::Multimedia
    pybind11::embed
    Python3::Python)
endif()
