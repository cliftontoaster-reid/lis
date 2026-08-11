#!/usr/bin/env bash
set -euo pipefail

COVERAGE=0
for arg in "$@"; do
    case "$arg" in
        --coverage)
            COVERAGE=1
            ;;
        --cross)
            echo "==> [install-macos] --cross is not applicable on macOS; skipping."
            exit 0
            ;;
        *)
            echo "Usage: $0 [--coverage] [--cross]" >&2
            exit 1
            ;;
    esac
done

if ! xcode-select -p >/dev/null 2>&1; then
    echo "==> [install-macos] Xcode Command Line Tools not found; running xcode-select --install"
    echo "==> [install-macos] NOTE: this requires interactive confirmation in a GUI dialog; on CI runners CLT is already installed."
    xcode-select --install || true
fi

if ! command -v brew >/dev/null 2>&1; then
    echo "==> [install-macos] Installing Homebrew (non-interactive)"
    NONINTERACTIVE=1 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

for prefix in /opt/homebrew /usr/local; do
    if [ -x "$prefix/bin/brew" ] && ! command -v brew >/dev/null 2>&1; then
        echo "==> [install-macos] Adding Homebrew ($prefix) to PATH"
        export PATH="$prefix/bin:$PATH"
    fi
done

echo "==> [install-macos] Installing base toolchain via Homebrew"
brew install cmake ninja git ccache unzip openssl pkg-config

echo "==> [install-macos] Note: Homebrew openssl is keg-only and is NOT added to PATH; CMake's find_package finds it via the standard brew paths. No PATH changes needed."

if [ "$COVERAGE" -eq 1 ]; then
    echo "==> [install-macos] Installing gcovr for coverage"
    if ! brew install gcovr; then
        echo "==> [install-macos] brew install gcovr failed; falling back to pip"
        if ! python3 -m pip install --break-system-packages gcovr 2>/dev/null; then
            if ! python3 -m pip install gcovr 2>/dev/null; then
                echo "==> [install-macos] WARNING: could not install gcovr; continuing without coverage" >&2
            fi
        fi
    fi
fi

echo "==> [install-macos] Installed tool versions:"
cmake --version | head -n 1
ninja --version
git --version
ccache --version | head -n 1
clang --version | head -n 1
gcc --version | head -n 1
