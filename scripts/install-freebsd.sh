#!/usr/bin/env bash
set -euo pipefail

COVERAGE=0
for arg in "$@"; do
    case "$arg" in
        --coverage)
            COVERAGE=1
            ;;
        --cross)
            echo "==> [install-freebsd] --cross is not applicable on FreeBSD; skipping."
            exit 0
            ;;
        *)
            echo "Usage: $0 [--coverage] [--cross]" >&2
            exit 1
            ;;
    esac
done

if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
else
    SUDO="sudo"
fi

echo "==> [install-freebsd] Installing base toolchain via pkg"
$SUDO pkg update -f
$SUDO pkg install -y cmake ninja ccache git unzip openssl pkgconf curl gnupg

echo "==> [install-freebsd] Using base clang (version 18 on FreeBSD 14) as the C++23 compiler"
echo "==> [install-freebsd] $(cc --version | head -n 1)"

if [ "$COVERAGE" -eq 1 ]; then
    echo "==> [install-freebsd] Installing gcovr for coverage (best effort)"
    if ! $SUDO pkg install -y py3-gcovr; then
        echo "==> [install-freebsd] pkg install py3-gcovr failed; falling back to pip"
        if ! python3 -m pip install --break-system-packages gcovr 2>/dev/null; then
            if ! python3 -m pip install gcovr 2>/dev/null; then
                echo "==> [install-freebsd] WARNING: could not install gcovr; continuing without coverage" >&2
            fi
        fi
    fi
fi

echo "==> [install-freebsd] Installed tool versions:"
cc --version | head -n 1
cmake --version | head -n 1
ninja --version
git --version
ccache --version
