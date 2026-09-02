#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Configuration Variables
PACKAGE_NAME="io.github.x6carbon12.vermilion"
QT_CMAKE="$HOME/Qt/6.11.2/android_arm64_v8a/bin/qt-cmake"
ANDROID_SDK="$HOME/android/sdk"
ANDROID_NDK="$HOME/android/sdk/ndk/26.1.10909125"

# Ensure at least a target is provided
if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <android|linux> [clean] [clear] [open]"
  exit 1
fi

TARGET="$1"
shift

CLEAN=0
CLEAR=0
OPEN=0
INSTALL=0

for arg in "$@"; do
  case "$arg" in
    clean) CLEAN=1 ;;
    clear) CLEAR=1 ;;
    open)  OPEN=1 ;;
    *) 
      echo "Error: Unknown argument '$arg'"
      echo "Usage: $0 <android|linux> [clean] [clear] [open]"
      exit 1 
      ;;
  esac
done

if [ "$TARGET" = "android" ]; then
  BUILD_DIR="build/android"

  if [ "$CLEAN" -eq 1 ]; then
    echo "Starting clean configuration for Android..."
    rm -rf $BUILD_DIR
    "$QT_CMAKE" \
      -DANDROID_SDK_ROOT="$ANDROID_SDK" \
      -DANDROID_NDK_ROOT="$ANDROID_NDK" \
      -DCMAKE_BUILD_TYPE=Debug \
      -S . \
      -B "$BUILD_DIR" \
      -GNinja
  fi

  echo "Building Android target..."
  cmake --build "$BUILD_DIR"

  if [ "$CLEAR" -eq 1 ]; then
    echo "Clearing application data..."
    adb shell pm clear "$PACKAGE_NAME" 2>/dev/null || true
  fi

  if [ "$INSTALL" -eq 1 ]; then
    echo "Installing APK..."
    adb install -r "$BUILD_DIR/android-build/build/outputs/apk/debug/android-build-debug.apk"
  fi

  if [ "$OPEN" -eq 1 ]; then
    echo "Launching $PACKAGE_NAME..."
    adb shell monkey -p "$PACKAGE_NAME" 1

    echo "Attaching logcat..."
    sleep 0.5
    adb logcat --pid=$(adb shell pidof -s "$PACKAGE_NAME")
  fi

elif [ "$TARGET" = "linux" ]; then
  BUILD_DIR="build/linux"

  if [ "$CLEAN" -eq 1 ]; then
    echo "Starting clean configuration for Linux..."
    rm -rf $BUILD_DIR
    cmake -B $BUILD_DIR
  fi

  echo "Building Linux target..."
  cmake --build "$BUILD_DIR"

  if [ "$OPEN" -eq 1 ]; then
    echo "Launching Linux application..."
    ./"$BUILD_DIR"/vermilion
  fi

else
  echo "Error: Unsupported target '$TARGET'. Must be 'android' or 'linux'."
  exit 1
fi
