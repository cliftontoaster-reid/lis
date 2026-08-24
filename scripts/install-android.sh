#!/usr/bin/env bash
set -euo pipefail

ANDROID_NDK_VERSION="${ANDROID_NDK_VERSION:-r27c}"
ANDROID_NDK_HOME="${ANDROID_NDK_HOME:-$HOME/android-ndk}"
DOWNLOAD_URL="https://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux.zip"

zlib_header_found=0
for header in /usr/include/zlib.h /usr/local/include/zlib.h /usr/include/*/zlib.h; do
    if [ -f "$header" ]; then
        zlib_header_found=1
        break
    fi
done

if ! command -v curl >/dev/null 2>&1 || ! command -v gpg >/dev/null 2>&1 || [ "$zlib_header_found" -eq 0 ]; then
    echo "==> [install-android] Installing required curl/gnupg/zlib packages"
    if [ "$(id -u)" -eq 0 ]; then SUDO=""; else SUDO="sudo"; fi
    installed=0
    if command -v apt-get >/dev/null 2>&1; then
        $SUDO apt-get update
        $SUDO apt-get install -y curl gnupg zlib1g-dev
        installed=1
    elif command -v dnf >/dev/null 2>&1; then
        $SUDO dnf install -y curl gnupg2 zlib-devel
        installed=1
    elif command -v yum >/dev/null 2>&1; then
        $SUDO yum install -y curl gnupg2 zlib-devel
        installed=1
    elif command -v apk >/dev/null 2>&1; then
        $SUDO apk add --no-cache curl gnupg zlib-dev
        installed=1
    elif command -v pacman >/dev/null 2>&1; then
        $SUDO pacman -S --noconfirm --needed curl gnupg zlib
        installed=1
    elif command -v zypper >/dev/null 2>&1; then
        $SUDO zypper --non-interactive install curl gpg2 zlib-devel
        installed=1
    fi
    if [ "$installed" -eq 0 ]; then
        echo "==> [install-android] ERROR: no supported package manager found for curl, gnupg, and zlib" >&2
        exit 1
    fi
fi

if ! command -v curl >/dev/null 2>&1 || ! command -v gpg >/dev/null 2>&1; then
    echo "==> [install-android] ERROR: curl and gpg are required but were not installed" >&2
    exit 1
fi

zlib_header_found=0
for header in /usr/include/zlib.h /usr/local/include/zlib.h /usr/include/*/zlib.h; do
    if [ -f "$header" ]; then
        zlib_header_found=1
        break
    fi
done
if [ "$zlib_header_found" -eq 0 ]; then
    echo "==> [install-android] ERROR: zlib development headers are required but were not installed" >&2
    exit 1
fi

if [ -f "$ANDROID_NDK_HOME/source.properties" ] && [ -f "$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake" ]; then
    echo "==> [install-android] NDK already installed at $ANDROID_NDK_HOME"
    sed -n 's/^Pkg\.Revision *= *//p' "$ANDROID_NDK_HOME/source.properties"
    exit 0
fi

echo "==> [install-android] Downloading NDK $ANDROID_NDK_VERSION"
TMPDIR_PREFIX="${XDG_CACHE_HOME:-$HOME/.cache}/install-android"
mkdir -p "$TMPDIR_PREFIX"
WORKDIR="$(mktemp -d "$TMPDIR_PREFIX/android-ndk.XXXXXX")"
trap 'rm -rf "$WORKDIR"' EXIT

curl -fL --retry 3 -o "$WORKDIR/android-ndk.zip" "$DOWNLOAD_URL"

echo "==> [install-android] Extracting NDK $ANDROID_NDK_VERSION"
if command -v unzip >/dev/null 2>&1; then
    unzip -q "$WORKDIR/android-ndk.zip" -d "$WORKDIR/extract"
else
    echo "==> [install-android] unzip not found; using python3 -m zipfile"
    python3 -m zipfile -e "$WORKDIR/android-ndk.zip" "$WORKDIR/extract"
fi

EXTRACTED="$WORKDIR/extract/android-ndk-${ANDROID_NDK_VERSION}"
if [ ! -d "$EXTRACTED" ]; then
    echo "==> [install-android] ERROR: expected extracted directory $EXTRACTED not found" >&2
    exit 1
fi

mkdir -p "$(dirname "$ANDROID_NDK_HOME")"
if [ "$EXTRACTED" != "$ANDROID_NDK_HOME" ]; then
    if [ -e "$ANDROID_NDK_HOME" ]; then
        echo "==> [install-android] ERROR: $ANDROID_NDK_HOME already exists but is not a valid NDK" >&2
        exit 1
    fi
    mv "$EXTRACTED" "$ANDROID_NDK_HOME"
fi

if [ ! -f "$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake" ]; then
    echo "==> [install-android] ERROR: NDK toolchain file missing after install" >&2
    exit 1
fi

echo "==> [install-android] Installed NDK $ANDROID_NDK_VERSION at $ANDROID_NDK_HOME"
sed -n 's/^Pkg\.Revision *= *//p' "$ANDROID_NDK_HOME/source.properties"

if [ -n "${GITHUB_ENV:-}" ] && [ -f "$GITHUB_ENV" ]; then
    echo "ANDROID_NDK_HOME=$ANDROID_NDK_HOME" >> "$GITHUB_ENV"
    echo "ANDROID_NDK_ROOT=$ANDROID_NDK_HOME" >> "$GITHUB_ENV"
else
    echo "$ANDROID_NDK_HOME"
fi
