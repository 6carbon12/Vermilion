cmake_minimum_required(VERSION 3.16)
project(Vermilion VERSION 0.0 LANGUAGES CXX)

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

find_package(Qt6 6.11 REQUIRED COMPONENTS Core Qml Quick)
qt_standard_project_setup(REQUIRES 6.11)

set(APP_NAME vermilion)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Source Files
qt_add_executable(${APP_NAME} main.cpp)
# QML Files
qt_add_qml_module(${APP_NAME} URI ${PROJECT_NAME} VERSION 1.0 QML_FILES Main.qml)

# `slots` is a macro used by QT
# but `slots` is used as a variable in Python C API
# So we disable that keywork with this:
target_compile_definitions(${APP_NAME} PRIVATE QT_NO_KEYWORDS)

if(ANDROID)
  # Setup
  set(PYTHON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/platform/android/python")
  set(PYTHON_INC_DIR "${PYTHON_DIR}/include/python3.11")
  set(PYTHON_LIB_DIR "${PYTHON_DIR}/lib/arm64-v8a")

  # Links and Includes
  target_include_directories(${APP_NAME} PRIVATE "${PYTHON_INC_DIR}")

  target_link_libraries(${APP_NAME}
    PRIVATE Qt6::Quick
    PRIVATE pybind11::embed
    PRIVATE "${PYTHON_LIB_DIR}/libpython3.11.so"
    PRIVATE "${PYTHON_LIB_DIR}/libffi.so"
    PRIVATE "${PYTHON_LIB_DIR}/libcrypto.so"
    PRIVATE "${PYTHON_LIB_DIR}/libssl.so"
    PRIVATE "${PYTHON_LIB_DIR}/libsqlite3.so"
  )

  # Other configuration
  set_property(TARGET ${APP_NAME} PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android-template")
  set_property(TARGET ${APP_NAME} PROPERTY QT_ANDROID_APP_NAME "Vermilion")
  set_property(TARGET ${APP_NAME} PROPERTY QT_ANDROID_PACKAGE_NAME "io.github.x6carbon12.vermilion")
  set_property(TARGET ${APP_NAME} PROPERTY
    QT_ANDROID_EXTRA_LIBS
    "${PYTHON_LIB_DIR}/libpython3.11.so"
    "${PYTHON_LIB_DIR}/libffi.so"
    "${PYTHON_LIB_DIR}/libcrypto.so"
    "${PYTHON_LIB_DIR}/libssl.so"
    "${PYTHON_LIB_DIR}/libsqlite3.so"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/android/libs/libqjs.so"
  )

else()
  find_package(Python3 COMPONENTS Development.Embed REQUIRED)
  target_link_libraries(vermilion PRIVATE
    Qt6::Quick
    pybind11::embed
    Python3::Python)
endif()
