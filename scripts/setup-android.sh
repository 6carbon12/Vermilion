#!/bin/env /bin/bash

set -euo pipefail

P4A_DIR="${HOME}/.local/share/python-for-android/build"
P4A_PYTHON_BUILD="${P4A_DIR}/other_builds/python3/arm64-v8a__ndk_target_27/python3"
ANDROID_PKG_STAGE="${1:-android_pkg_stage}"

echo "Preparing staging directory at: ${ANDROID_PKG_STAGE}"
rm -rf "${ANDROID_PKG_STAGE}/assets"
rm -rf "${ANDROID_PKG_STAGE}/libs/arm64-v8a"
mkdir -p "${ANDROID_PKG_STAGE}/assets"
mkdir -p "${ANDROID_PKG_STAGE}/libs/arm64-v8a"

PYTHON_SO="${P4A_PYTHON_BUILD}/android-build/libpython3.11.so"
if [ -f "$PYTHON_SO" ]; then
    echo "Copying libpython3.11.so..."
    cp -r "$PYTHON_SO" "${ANDROID_PKG_STAGE}/libs/arm64-v8a/"
else
    echo "Error: Python source directory not found at $PYTHON_SO" >&2
    exit 1
fi

PYTHON_SRC="${P4A_PYTHON_BUILD}/android-build/android-root/lib/python3.11"
if [ -d "$PYTHON_SRC" ]; then
    echo "Copying python3.11..."
    cp -r "$PYTHON_SRC" "${ANDROID_PKG_STAGE}/assets/python311"
else
    echo "Error: Python source directory not found at $PYTHON_SRC" >&2
    exit 1
fi

INSTALLS_DIR="${P4A_DIR}/python-installs/embed/arm64-v8a"
if [ -d "$INSTALLS_DIR" ]; then
    echo "Copying python-installs packages..."
    cp -r "$INSTALLS_DIR/"* "${ANDROID_PKG_STAGE}/assets/"
else
    echo "Error: Python installs directory not found at $INSTALLS_DIR" >&2
    exit 1
fi

DYNLOAD_DIR="${P4A_PYTHON_BUILD}/android-build/android-root/lib/python3.11/lib-dynload"
if [ -d "$DYNLOAD_DIR" ]; then
    echo "Copying lib-dynload .so files..."
    cp "$DYNLOAD_DIR"/*.so "${ANDROID_PKG_STAGE}/libs/arm64-v8a/"
else
    echo "Error: Dynload directory not found at $DYNLOAD_DIR" >&2
    exit 1
fi

PYTHON_LIBS_DIR="${P4A_DIR}/libs_collections/embed/arm64-v8a/"
if [ -d "$PYTHON_LIBS_DIR" ]; then
    echo "Copying python libraries..."
    cp "$PYTHON_LIBS_DIR"/*.so "${ANDROID_PKG_STAGE}/libs/arm64-v8a/"
else
    echo "Error: Python library directory not found at ${PYTHON_LIBS_DIR}" >&2
    exit 1
fi

PYTHON_INC_DIR="${P4A_DIR}/other_builds/python3/arm64-v8a__ndk_target_27/python3/Include"
if [ -d "$PYTHON_INC_DIR" ]; then
    echo "Copying python headers..."
    mkdir -p "${ANDROID_PKG_STAGE}/include/"
    cp -r "$PYTHON_INC_DIR"/* "${ANDROID_PKG_STAGE}/include/"
else
  echo "Error: Python headers directory not found at ${PYTHON_INC_DIR}" >&2
    exit 1
fi

echo "Performing cleanup tasks..."

# Remove specified files and directories from assets/python311/
PYTHON311_DIR="${ANDROID_PKG_STAGE}/assets/python311"
if [ -d "$PYTHON311_DIR" ]; then
  rm -rf \
    "$PYTHON311_DIR/test" \
    "$PYTHON311_DIR/lib-dynload" \
    "$PYTHON311_DIR/ensurepip" \
    "$PYTHON311_DIR/pydoc_data" \
    "$PYTHON311_DIR/tkinter" \
    "$PYTHON311_DIR/turtledemo" \
    "$PYTHON311_DIR/idlelib" \
    "$PYTHON311_DIR/unittest" \
    "$PYTHON311_DIR/diskutils" \
    "$PYTHON311_DIR/lib2to3" \
    "$PYTHON311_DIR/venv" \
    "$PYTHON311_DIR/turtle.py" \
    "$PYTHON311_DIR/pydoc.py" \
    "$PYTHON311_DIR/doctest.py" \
    "$PYTHON311_DIR/pdb.py" \
    "$PYTHON311_DIR/cProfile.py" \
    "$PYTHON311_DIR/profile.py" \
    "$PYTHON311_DIR/antigravity.py" \
    "$PYTHON311_DIR/this.py"
fi

# Remove all __pycache__ directories in assets/
find "${ANDROID_PKG_STAGE}/assets" -type d -name "__pycache__" -exec rm -rf {} +

# Remove all dist-info directories in assets/
find "${ANDROID_PKG_STAGE}/assets" -type d -name "*.dist-info" -exec rm -rf {} +

# Clean yt_dlp extractors if the directory exists
YT_EXTRACTOR_DIR="${ANDROID_PKG_STAGE}/assets/yt_dlp/extractor"
if [ -d "$YT_EXTRACTOR_DIR" ]; then
  echo "Cleaning yt_dlp extractors..."
  cd "$YT_EXTRACTOR_DIR"
  find . -maxdepth 1 -type f -name "*.py" \
    ! -name "__init__.py" \
    ! -name "common.py" \
    ! -name "commonprotocols.py" \
    ! -name "extractors.py" \
    ! -name "_extractors.py" \
    ! -name "lazy_extractors.py" \
    ! -name "generic.py" \
    ! -name "afreecatv.py" \
    ! -name "adobepass.py" \
    ! -name "openload.py" \
    -delete
  cd - > /dev/null
fi

rm -f "${ANDROID_PKG_STAGE}/libs/arm64-v8a/libpythonbin.so"

echo "Patching OpenSSL libraries to prevent duplication with Qt..."
LIBS_DIR="${ANDROID_PKG_STAGE}/libs/arm64-v8a"
if [ -f "${LIBS_DIR}/libcrypto.so" ] && [ -f "${LIBS_DIR}/libssl.so" ]; then
  cd "${LIBS_DIR}"

  mv libcrypto.so libcrypto_3.so
  mv libssl.so libssl_3.so

  patchelf --set-soname libcrypto_3.so libcrypto_3.so
  patchelf --set-soname libssl_3.so libssl_3.so
  patchelf --replace-needed libcrypto.so libcrypto_3.so libssl_3.so

  for lib in *.so; do
    if [[ "$lib" == "libcrypto_3.so" || "$lib" == "libssl_3.so" ]]; then
      continue
    fi

    if patchelf --print-needed "$lib" | grep -q "libcrypto.so"; then
      patchelf --replace-needed libcrypto.so libcrypto_3.so "$lib"
    fi

    if patchelf --print-needed "$lib" | grep -q "libssl.so"; then
      patchelf --replace-needed libssl.so libssl_3.so "$lib"
    fi
  done

  cd - > /dev/null
else
  echo "Warning: libcrypto.so or libssl.so not found in ${LIBS_DIR}, skipping patch."
fi

echo "Android staging and cleanup completed successfully!"
