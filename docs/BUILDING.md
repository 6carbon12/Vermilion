# Building and Compiling Guide

This document outlines the steps required to build the application from source, compile the necessary binaries (`quickjs-ng`), and prepare the custom `python-for-android` (p4a) packages with patched OpenSSL dependencies.

---

## Prerequisites

Ensure you have the following tools installed on your development environment:
* A modern Linux environment (Arch Linux recommended)
* Android SDK and NDK
* CMake (v3.22+)
* Qt 6 development libraries
* Python 3 with `python-for-android` and build dependencies (`pybind11`, `yt-dlp`, etc.)
* `patchelf` (required for ELF symbol/SONAME modifications)

---

## Android

> [!NOTE]
> You can skip step 1 and 3 if you run `build.sh` directly, which fetches precompiled binaries from releases.

### 1. Compiling `quickjs-ng` for Android

To compile `quickjs-ng` for the Android target architecture using the Android NDK toolchain:

1. Clone the `quickjs-ng` repository.
2. Configure the build using CMake with your Android NDK toolchain file:

```bash
git clone https://github.com/quickjs-ng/quickjs-ng.git
cd quickjs-ng
mkdir build-android && cd build-android

cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24 \
    -DCMAKE_BUILD_TYPE=Release

cmake --build . -j$(nproc)
```

### 2. Getting python-for-android files

1. Create a python virtual environment for version `3.11.11` and install `python-for-android`.
```bash
mkdir p4a && cd p4a
pyenv local 3.11.11
python -m venv venv
source venv/bin/activate
pip install python-for-android
```
2. Compile the actual Android dependencies.
```bash
p4a create \
    --bootstrap=empty \
    --requirements="hostpython3==3.11.11,python3==3.11.11,certifi,requests,yt-dlp,yt-dlp-ejs,ytmusicapi" \
    --arch=arm64-v8a \
    --sdk_dir=$ANDROID_SDK_HOME \
    --ndk_dir=$ANDROID_NDK_HOME/27.2.12479018 \
    --dist_name=embed \
    --android-api 36 \
    --ndk-api 27
```

> [!NOTE]
> Make sure `$ANDROID_SDK_HOME` and `$ANDROID_NDK_HOME` are setup properly.

### 3. Creating patched ssl and crypto libraries

*Qt requires these libraries on Android.*

1. Get actual compiled libs from `python-for-android`
```bash
cp ~/.local/share/python-for-android/build/libs_collections/embed/arm64-v8a/libssl.so /tmp/
cp ~/.local/share/python-for-android/build/libs_collections/embed/arm64-v8a/libcrypto.so /tmp/
```
2. Path them with new `SONAME`
```bash
patchelf --set-soname libcrypto_3.so /tmp/libcrypto.so
patchelf --set-soname libssl_3.so /tmp/libssl.so
patchelf --replace-needed libcrypto.so libcrypto_3.so /tmp/libssl.so
```

### 4. Final build execution

```bash
./build.sh android
```
