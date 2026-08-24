#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<'EOF'
Usage: install-msys2.sh [--coverage] [--cross]

Installs the MinGW-w64 (GCC) toolchain for the Windows MinGW CI leg.
Runs inside an MSYS2 environment; requires pacman.

Flags:
  --coverage   Not applicable inside MSYS2 for this project (warning, ignored)
  --cross      Not applicable inside MSYS2 for this project (warning, ignored)
  -h, --help   Show this help
EOF
}

for arg in "$@"; do
    case "$arg" in
        --coverage)
            printf '==> [install-msys2] WARNING: --coverage is not applicable inside MSYS2 for this project; continuing.\n'
            ;;
        --cross)
            printf '==> [install-msys2] WARNING: --cross is not applicable inside MSYS2 for this project; continuing.\n'
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            printf '==> [install-msys2] ERROR: unknown argument: %s\n' "$arg" >&2
            usage >&2
            exit 1
            ;;
    esac
done

if ! command -v pacman >/dev/null 2>&1; then
    printf '==> [install-msys2] ERROR: pacman not found.\n' >&2
    printf '==> [install-msys2] This script must run inside an MSYS2 shell (e.g. the default shell of the msys2/setup-msys2 GitHub Action).\n' >&2
    exit 1
fi

printf '==> [install-msys2] Installing MinGW-w64 toolchain via pacman\n'
pacman -S --noconfirm --needed \
    base-devel \
    git \
    mingw-w64-x86_64-toolchain \
    mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-ninja \
    mingw-w64-x86_64-ccache \
    mingw-w64-x86_64-openssl \
    mingw-w64-x86_64-curl \
    mingw-w64-x86_64-zlib \
    gnupg \
    curl

printf '==> [install-msys2] Installed tool versions:\n'
gcc --version
cmake --version
ninja --version
ccache --version

printf '==> [install-msys2] Done.\n'
