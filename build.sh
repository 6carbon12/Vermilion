#!/bin/bash

set -e

PACKAGE_NAME="io.github.x6carbon12.vermilion"
QT_CMAKE="$HOME/Qt/6.11.2/android_arm64_v8a/bin/qt-cmake"
ANDROID_SDK="${ANDROID_SDK:-$HOME/android/sdk}"
ANDROID_NDK="${ANDROID_NDK:-$HOME/android/sdk/ndk}"

USEAGE="Usage: $0 <android|linux> [clean] [clear] [install] [open]"

if [ "$#" -lt 1 ]; then
  echo $USEAGE
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
    install)  INSTALL=1 ;;
    *)
      echo "Error: Unknown argument '$arg'"
      echo $USEAGE
      exit 1
      ;;
  esac
done

updateLspFiles() {
	echo "Updating .qmlls.ini"
	sed -E '/^\[.*(SLASH).*\]/d' $1/.qt/.qmlls.build.ini > .qmlls.ini

	echo "Updating compile_commands.json"
	cp $1/compile_commands.json ./compile_commands.json
}

if [ "$TARGET" = "android" ]; then
  BUILD_DIR="build/android"
  LOGFILE="$BUILD_DIR/build.log"

  rm $LOGFILE >/dev/null 2>&1 || true
  mkdir -p $BUILD_DIR
  touch $LOGFILE

  if [ "$CLEAN" -eq 1 ]; then
    echo "Cleaning build directory."
    rm -rf $BUILD_DIR
    mkdir -p $BUILD_DIR
    touch $LOGFILE
    echo "Writing new build files."
    "$QT_CMAKE" \
      -DANDROID_SDK_ROOT="$ANDROID_SDK" \
      -DANDROID_NDK_ROOT="$ANDROID_NDK/27.2.12479018" \
      -DCMAKE_BUILD_TYPE=Debug \
      -S . \
      -B "$BUILD_DIR" \
      -GNinja >> $LOGFILE
  fi

  echo "Building Android target..."
  cmake --build "$BUILD_DIR" 2>&1 | tee -a "$LOGFILE" | grep --line-buffered -E '^\['

  if [ ${PIPESTATUS[0]} -ne 0 ]; then
    echo "Build Failed..."
    bat "$LOGFILE"
    exit 1
  fi

	updateLspFiles $BUILD_DIR

  if [ "$CLEAR" -eq 1 ]; then
    echo "Clearing application data..."
    adb shell pm clear "$PACKAGE_NAME" >> $LOGFILE 2>/dev/null || true
  fi

  if [ "$INSTALL" -eq 1 ]; then
    echo "Installing APK..."
    adb install -r "$BUILD_DIR/android-build/vermilion.apk" >> $LOGFILE 2>> $LOGFILE
  fi

  if [ "$OPEN" -eq 1 ]; then
    echo "Launching $PACKAGE_NAME..."
    adb shell monkey -p "$PACKAGE_NAME" 1 >> $LOGFILE 2>> $LOGFILE

    echo "Attaching logcat..."
    sleep 0.5
    adb logcat --pid=$(adb shell pidof -s "$PACKAGE_NAME")
  fi

elif [ "$TARGET" = "linux" ]; then
  BUILD_DIR="build/linux"
  $LOGFILE="$BUILD_DIR/build.log"

  rm $LOGFILE >/dev/null 2>&1 || true
  mkdir -p $BUILD_DIR
  touch $LOGFILE

  if [ "$CLEAN" -eq 1 ]; then
    echo "Cleaning build directory."
    rm -rf $BUILD_DIR
    mkdir -p $BUILD_DIR
    touch $LOGFILE
    echo "Writing new build files."
    cmake -B $BUILD_DIR >> $LOGFILE
  fi

  echo "Building Linux target..."
  cmake --build "$BUILD_DIR" >> $LOGFILE

	updateLspFiles $BUILD_DIR

  if [ "$OPEN" -eq 1 ]; then
    echo "Launching Linux application..."
    ./"$BUILD_DIR"/vermilion
  fi

else
  echo "Error: Unsupported target '$TARGET'. Must be 'android' or 'linux'."
  exit 1
fi
