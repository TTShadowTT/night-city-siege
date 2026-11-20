#!/bin/sh
# Builds the native Linux game from this source folder and installs it for the current user (no root needed).
#   sh scripts/linux_from_source.sh              build and install
#   NO_INSTALL=1 sh scripts/linux_from_source.sh only build (into build/linux)
# Needs gcc, the C library headers and the X11 headers (Debian / Ubuntu: sudo apt install build-essential libx11-dev libxext-dev).
# Without the headers on Debian / Ubuntu it downloads them into ~/.cache/neon-edge-build itself (no root needed).
set -e
ROOT=$(cd "$(dirname "$0")/.." && pwd)
OUT="$ROOT/build/linux/NeonEdge"
command -v gcc >/dev/null 2>&1 || { echo "gcc is missing - install it first (Debian / Ubuntu: sudo apt install build-essential)" >&2; exit 1; }
echo '#include <math.h>
#include <sys/types.h>' | gcc -E - >/dev/null 2>&1 ||
    { echo "The C library headers are missing - install them first (Debian / Ubuntu: sudo apt install build-essential)" >&2; exit 1; }

INC=""
if ! echo '#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>' | gcc -E - >/dev/null 2>&1; then
    command -v apt-get >/dev/null 2>&1 || { echo "The X11 headers are missing - install libx11-dev and libxext-dev (or your system's X11 development package)" >&2; exit 1; }
    B="$HOME/.cache/neon-edge-build"
    if [ ! -f "$B/inc/usr/include/X11/extensions/XShm.h" ]; then
        echo "Downloading the X11 headers into $B (nothing gets installed)..."
        rm -rf "$B/inc" "$B/debs"; mkdir -p "$B/inc" "$B/debs"
        (cd "$B/debs" && apt-get download libx11-dev libxext-dev x11proto-dev >/dev/null)
        for d in "$B"/debs/*.deb; do dpkg-deb -x "$d" "$B/inc"; done
    fi
    INC="-I$B/inc/usr/include"
fi

# link against the libraries the desktop already has (the .so.6 files), so no -dev package is needed for them either
lib() { for d in /usr/lib/x86_64-linux-gnu /usr/lib64 /usr/lib /lib/x86_64-linux-gnu /usr/lib/aarch64-linux-gnu; do
            [ -f "$d/$1" ] && { echo "$d/$1"; return; }; done; echo "-l:$1"; }

rm -rf "$OUT"; mkdir -p "$OUT"
echo "Compiling..."
gcc -O2 -Wall -fno-common -I"$ROOT/src" -I"$ROOT/src/core" -I"$ROOT/src/platform/linux" $INC \
    -o "$OUT/neon-edge-bin" "$ROOT"/src/core/*.c "$ROOT"/src/platform/linux/*.c "$(lib libX11.so.6)" "$(lib libXext.so.6)" -lm -ldl -lpthread -s
cp "$ROOT/src/platform/linux/neon-edge" "$ROOT/src/platform/linux/install.sh" "$ROOT/src/platform/linux/uninstall.sh" "$OUT/"
cp "$ROOT/assets/icons/night-city-siege.png" "$OUT/neon-edge.png"
chmod 755 "$OUT/neon-edge-bin" "$OUT/neon-edge" "$OUT/install.sh" "$OUT/uninstall.sh"
echo "Built $OUT/neon-edge-bin"
[ -n "$NO_INSTALL" ] || sh "$OUT/install.sh"
