#!/usr/bin/env bash
set -euo pipefail

CROSS=0
COVERAGE=0

for arg in "$@"; do
  case "$arg" in
    --cross)
      CROSS=1
      ;;
    --coverage)
      COVERAGE=1
      ;;
    *)
      echo "usage: $0 [--cross] [--coverage]" >&2
      exit 1
      ;;
  esac
done

if [ "$(id -u)" -eq 0 ]; then
  SUDO=""
else
  SUDO="sudo"
fi

PM=""

detect_pm() {
  local id like
  id=""
  like=""
  if [ -r /etc/os-release ]; then
    . /etc/os-release
    id="${ID:-}"
    like="${ID_LIKE:-}"
  fi
  case " $id $like " in
    *debian*|*ubuntu*|*raspbian*|*linuxmint*)
      PM="apt"
      ;;
    *rhel*|*centos*|*rocky*|*alma*|*ol*|*amzn*|*fedora*)
      if command -v dnf >/dev/null 2>&1; then
        PM="dnf"
      else
        PM="yum"
      fi
      ;;
    *arch*|*manjaro*)
      PM="pacman"
      ;;
    *alpine*)
      PM="apk"
      ;;
    *suse*)
      PM="zypper"
      ;;
  esac
  if [ -z "$PM" ]; then
    for p in apt dnf yum pacman apk zypper; do
      if command -v "$p" >/dev/null 2>&1; then
        PM="$p"
        break
      fi
    done
  fi
}

detect_pm

UPDATE_CMD=""
INSTALL_CMD=""
BASE_PKGS=""
CROSS_PKGS=""

case "$PM" in
  apt)
    UPDATE_CMD="$SUDO apt-get update"
    INSTALL_CMD="$SUDO apt-get install -y"
    BASE_PKGS="cmake ninja-build git ccache unzip gcc g++ pkg-config libssl-dev"
    CROSS_PKGS="g++-aarch64-linux-gnu g++-riscv64-linux-gnu"
    ;;
  dnf)
    INSTALL_CMD="$SUDO dnf install -y"
    BASE_PKGS="cmake ninja-build git ccache unzip gcc gcc-c++ pkgconf-pkg-config openssl-devel"
    CROSS_PKGS="gcc-aarch64-linux-gnu gcc-riscv64-linux-gnu binutils-aarch64-linux-gnu binutils-riscv64-linux-gnu"
    ;;
  yum)
    INSTALL_CMD="$SUDO yum install -y"
    BASE_PKGS="cmake ninja-build git ccache unzip gcc gcc-c++ pkgconfig openssl-devel"
    CROSS_PKGS="gcc-aarch64-linux-gnu gcc-riscv64-linux-gnu binutils-aarch64-linux-gnu binutils-riscv64-linux-gnu"
    ;;
  pacman)
    INSTALL_CMD="$SUDO pacman -S --noconfirm --needed"
    BASE_PKGS="base-devel cmake ninja git ccache unzip gcc pkg-config openssl"
    ;;
  apk)
    UPDATE_CMD="$SUDO apk update"
    INSTALL_CMD="$SUDO apk add --no-cache"
    BASE_PKGS="cmake ninja git ccache unzip gcc g++ musl-dev pkgconf openssl-dev"
    ;;
  zypper)
    INSTALL_CMD="$SUDO zypper --non-interactive install"
    BASE_PKGS="cmake ninja git ccache unzip gcc gcc-c++ pkg-config libopenssl-devel"
    CROSS_PKGS="cross-aarch64-gcc14 cross-riscv64-gcc14"
    ;;
  *)
    echo "==> [install-linux] unsupported package manager: no apt/dnf/yum/pacman/apk/zypper detected" >&2
    exit 1
    ;;
esac

echo "==> [install-linux] Detected package manager: $PM"

if [ -n "$UPDATE_CMD" ]; then
  echo "==> [install-linux] Running $UPDATE_CMD"
  $UPDATE_CMD
fi

echo "==> [install-linux] Installing base toolchain via $PM"
$INSTALL_CMD $BASE_PKGS

if [ "$CROSS" -eq 1 ]; then
  if [ "$PM" = "pacman" ] || [ "$PM" = "apk" ]; then
    echo "==> [install-linux] warning: cross toolchains not in official repos, skipping" >&2
  elif [ -n "$CROSS_PKGS" ]; then
    echo "==> [install-linux] Installing cross toolchains via $PM"
    $INSTALL_CMD $CROSS_PKGS || {
      echo "==> [install-linux] warning: cross toolchain install failed, continuing" >&2
    }
  fi
fi

if [ "$COVERAGE" -eq 1 ]; then
  GCOVR_PKG=""
  case "$PM" in
    apt) GCOVR_PKG="gcovr" ;;
    dnf) GCOVR_PKG="gcovr" ;;
    yum) GCOVR_PKG="gcovr" ;;
    pacman) GCOVR_PKG="python-gcovr" ;;
    apk) GCOVR_PKG="py3-gcovr" ;;
    zypper) GCOVR_PKG="gcovr" ;;
  esac
  if [ -n "$GCOVR_PKG" ]; then
    echo "==> [install-linux] Installing gcovr ($GCOVR_PKG) via $PM"
    if $INSTALL_CMD "$GCOVR_PKG" >/dev/null 2>&1; then
      echo "==> [install-linux] gcovr installed from native package"
    elif python3 -m pip install --break-system-packages gcovr >/dev/null 2>&1; then
      echo "==> [install-linux] gcovr installed via pip (--break-system-packages)"
    elif python3 -m pip install gcovr >/dev/null 2>&1; then
      echo "==> [install-linux] gcovr installed via pip"
    else
      echo "==> [install-linux] warning: could not install gcovr, coverage reports unavailable" >&2
    fi
  fi
fi

echo "==> [install-linux] Installed versions:"
cmake --version | head -n1
ninja --version
git --version
ccache --version | head -n1
if command -v gcc >/dev/null 2>&1; then
  gcc --version | head -n1
fi
if command -v g++ >/dev/null 2>&1; then
  g++ --version | head -n1
fi
