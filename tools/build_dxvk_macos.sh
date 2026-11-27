#!/bin/bash
# P2CE's renderer on macOS arm64 (round 28, plan round F; route (a2), option 1
# in docs/fork/P2CE_RENDERER_DECISION.md): DXVK 1.10.x with the
# Gcenx/DXVK-macOS patches and dxvk-native's 1.x non-Windows build carried
# over (tools/dxvk/dxvk-1.10-macos-native.patch), on MoltenVK. Everything is
# fetched at pinned versions and built from source into deps/:
#   deps/glslang-src      glslang 16.6.0 (BSD-3/Apache 2.0): DXVK's build compiles its own shaders with it
#   deps/moltenvk-src     MoltenVK v1.4.2 (Apache 2.0): Vulkan on Metal
#   deps/dxvk-macos-src   DXVK (zlib): libdxvk_d3d11.dylib and libdxvk_dxgi.dylib
#   deps/meson-venv       meson 1.6.1 in its own Python venv
# Needs Xcode, cmake and ninja (Homebrew). Re-running skips what's built.
# Nothing here touches mtld3d or how the other games render.
#   tools/build_dxvk_macos.sh        prints the folder holding the two dylibs and MoltenVK's
set -euo pipefail
REPO="$(cd "$(dirname "$0")/.." && pwd)"
DEPS="${P2_DEPS:-$REPO/deps}"
GLSLANG_TAG=16.6.0
MOLTENVK_TAG=v1.4.2
DXVK_MACOS_URL=https://github.com/Gcenx/DXVK-macOS.git
DXVK_MACOS_REV=8f1e28deed3ad30802f7e1bdff428ec14e6e7817   # 1.10.x branch
DXVK_NATIVE_URL=https://github.com/Joshua-Ashton/dxvk-native.git
DXVK_NATIVE_REV=c8dc91fabd00cac11d697ccf07426e798393cd40  # master (native headers)
PATCH="$REPO/tools/dxvk/dxvk-1.10-macos-native.patch"
HOST="$DEPS/p2-host-deps-1-macos-arm64"
log() { echo "==> $*" >&2; }

command -v cmake >/dev/null && command -v ninja >/dev/null || { echo "needs cmake and ninja (brew install cmake ninja)" >&2; exit 1; }
SDL_INC="$HOST/include"; SDL_LIB="$HOST/lib"
[ -f "$SDL_INC/SDL2/SDL.h" ] || { SDL_INC=/opt/homebrew/include; SDL_LIB=/opt/homebrew/lib; }
[ -f "$SDL_INC/SDL2/SDL.h" ] || { echo "needs SDL2 (./setup.sh's host libraries, or brew install sdl2)" >&2; exit 1; }

# glslang
GLSLANG="$DEPS/glslang-src/build/StandAlone"
if [ ! -x "$GLSLANG/glslang" ]; then
    log "Building glslang $GLSLANG_TAG"
    [ -d "$DEPS/glslang-src" ] || git clone -q --depth 1 --branch "$GLSLANG_TAG" https://github.com/KhronosGroup/glslang.git "$DEPS/glslang-src"
    cmake -S "$DEPS/glslang-src" -B "$DEPS/glslang-src/build" -G Ninja -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_OPT=OFF -DGLSLANG_TESTS=OFF -DENABLE_HLSL=OFF >/dev/null
    ninja -C "$DEPS/glslang-src/build" glslang-standalone >/dev/null
fi

# MoltenVK
MVK="$DEPS/moltenvk-src/Package/Release/MoltenVK"
if [ ! -f "$MVK/dynamic/dylib/macOS/libMoltenVK.dylib" ]; then
    log "Building MoltenVK $MOLTENVK_TAG (several minutes)"
    [ -d "$DEPS/moltenvk-src" ] || git clone -q --depth 1 --branch "$MOLTENVK_TAG" https://github.com/KhronosGroup/MoltenVK.git "$DEPS/moltenvk-src"
    (cd "$DEPS/moltenvk-src" && ./fetchDependencies --macos >/dev/null && make macos >/dev/null)
fi

# meson
if [ ! -x "$DEPS/meson-venv/bin/meson" ]; then
    log "Installing meson in deps/meson-venv"
    python3 -m venv "$DEPS/meson-venv" && "$DEPS/meson-venv/bin/pip" install -q meson==1.6.1
fi

# DXVK
SRC="$DEPS/dxvk-macos-src"
if [ ! -f "$SRC/.p2-patched" ]; then
    log "Fetching DXVK 1.10 (Gcenx/DXVK-macOS) and dxvk-native's headers"
    rm -rf "$SRC" && git init -q "$SRC"
    git -C "$SRC" fetch -q --depth 1 "$DXVK_MACOS_URL" "$DXVK_MACOS_REV"
    git -C "$SRC" checkout -q FETCH_HEAD
    git -C "$SRC" fetch -q --depth 1 "$DXVK_NATIVE_URL" "$DXVK_NATIVE_REV"
    git -C "$SRC" checkout -q FETCH_HEAD -- include/native
    git -C "$SRC" apply "$PATCH"
    touch "$SRC/.p2-patched"
fi
export PATH="$GLSLANG:$DEPS/meson-venv/bin:$PATH"
export CPATH="$SDL_INC:$SDL_INC/SDL2:$MVK/include"
export LIBRARY_PATH="$SDL_LIB:$MVK/dynamic/dylib/macOS"
if [ ! -f "$SRC/build/build.ninja" ]; then
    log "Configuring DXVK (D3D11 and DXGI only)"
    (cd "$SRC" && meson setup build --buildtype=release -Denable_d3d9=false -Denable_d3d10=false \
        -Dnative_vulkan_lib=MoltenVK >/dev/null)
fi
log "Building DXVK"
ninja -C "$SRC/build" >/dev/null
OUT="$DEPS/dxvk-macos"
mkdir -p "$OUT"
cp "$SRC/build/src/d3d11/libdxvk_d3d11.dylib" "$SRC/build/src/dxgi/libdxvk_dxgi.dylib" \
   "$MVK/dynamic/dylib/macOS/libMoltenVK.dylib" "$OUT/"
# They find each other and MoltenVK beside them, and SDL2 where it was linked
# from: the same file make p2ce-launch hands P2CE (one SDL in the process).
for lib in "$OUT/libdxvk_d3d11.dylib" "$OUT/libdxvk_dxgi.dylib"; do
    install_name_tool -add_rpath @loader_path "$lib" 2>/dev/null || true
    install_name_tool -add_rpath "$SDL_LIB" "$lib" 2>/dev/null || true
    codesign --force -s - "$lib" 2>/dev/null
done
echo "$OUT"
