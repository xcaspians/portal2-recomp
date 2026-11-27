#!/bin/bash
# One-shot setup and build: checks the tools, downloads Valve's macOS
# libsteam_api.dylib (inside Steamworks.NET), writes config/local.json,
# recompiles each game and creates its app in ~/Applications: "Portal 2
# Natif.app", "Half-Life 2 Natif.app", "Portal Natif.app", "Portal Stories Mel
# Natif.app" and/or "Left 4 Dead 2 Natif.app" (all but Portal 2 experimental:
# docs/hl2.md, docs/portal1.md, docs/mel.md, docs/l4d2.md). The games, their
# Steam depots and files come from one table, tools/games.py (installer
# entries), which the Natif Installer reads too.
# With --ios, also builds, signs and installs the Sauce app on the iPhone or
# iPad (build/ios/device/Portal2.app), every game inside it.
# Host libraries (SDL2, FreeType, Fontconfig) come from Homebrew when it has
# them all, else from a prebuilt bundle downloaded from this project's GitHub
# releases (SHA-256 checked; tools/build_host_deps.sh makes it).
# P2_HOST_DEPS=homebrew or bundle forces one (P2_HOST_DEPS_URL: another copy). The Metal renderer's libd3d9metal
# is built when Rust (https://rustup.rs) is installed, else taken prebuilt from
# the same bundle. P2_NO_METAL=1 skips it: the game then uses OpenGL.
#
# usage: ./setup.sh [--game portal2|hl2|portal1|mel|l4d2|all] [--ios] [--puzzle-maker]
#        ./setup.sh [--ios] ["/path/to/Portal 2" ["/path/to/depot_661"]]
#        ./setup.sh --game hl2 ["/path/to/Half-Life 2" ["/path/to/depot_224"]]
#        ./setup.sh --game portal1 ["/path/to/Portal" ["/path/to/depot_404"]]
#        ./setup.sh --game mel ["/path/to/Portal Stories Mel" ["/path/to/depot_317404"]]
#        ./setup.sh --game l4d2 ["/path/to/Left 4 Dead 2" ["/path/to/depot_554"]]
#        ./setup.sh --detect [--game ...]   prints the games it would build, changes nothing
#
# Without --game, every game found on this Mac is built: the ones already in
# config/local.json, and the ones where Steam puts them (the game installed by
# Steam, and its Linux binaries fetched with the Steam console's
# download_depot command). --game all requires all of them. One checkout holds all
# games (build/game for Portal 2, build/hl2, build/portal1, build/mel and
# build/l4d2 for the others): adding one never rebuilds another.
# Paths are for one game; without --game, the folder's mod folder says which
# (hl2_complete/ Half-Life 2, portal/ Portal, portal_stories/ Mel, left4dead2/
# Left 4 Dead 2), anything else is Portal 2.
#
# Left 4 Dead 2 is local and offline only: every launch gets -insecure
# +sv_lan 1 -nomaster (tools/games.py launch_args), never Valve's servers.
#
# --puzzle-maker also builds Portal 2's map compilers (VBSP, VVIS, VRAD), which
# the Puzzle Maker needs to build a chamber: about ten more minutes and 220 MB
# in the app (docs/puzzlemaker.md).
#
# P2_APP_DIR puts the apps elsewhere than ~/Applications.
# Re-running is safe: finished steps are skipped and unchanged code is not
# recompiled. Downloads and iOS libraries go to ./deps (P2_DEPS overrides it).
# Saves go to ~/Library/Application Support/<app name>/userdata (config key
# user_dir). ./uninstall.sh removes the apps and, if asked, the saves.
set -euo pipefail
# Interrupted (Ctrl-C, or the installer's cancel, which signals bash alone):
# take the running python/clang down too. When bash leads its process group
# (Process on macOS gives it one), the whole group; else its children.
on_signal() {
    trap - INT TERM
    if [ "$(ps -o pgid= -p $$ | tr -d ' ')" = "$$" ]; then kill -TERM 0 2>/dev/null; else pkill -TERM -P $$ 2>/dev/null; fi
    exit 130
}
trap on_signal INT TERM

REPO="$(cd "$(dirname "$0")" && pwd)"
DEPS="${P2_DEPS:-$REPO/deps}"
CONFIG="$REPO/config/local.json"
IOS=0
DETECT=0
PUZZLE_MAKER=0
GAME_OPT=""
ARGS=()
while [ $# -gt 0 ]; do
    a="$1"; shift
    case "$a" in
        --ios) IOS=1 ;;
        --detect) DETECT=1 ;;
        --puzzle-maker) PUZZLE_MAKER=1 ;;
        --game) [ $# -gt 0 ] || { echo "--game needs a value: portal2, hl2, portal1, mel, l4d2 or all" >&2; exit 2; }
                GAME_OPT="$1"; shift ;;
        --game=*) GAME_OPT="${a#--game=}" ;;
        -h|--help) awk 'NR > 1 && !/^#/ {exit} NR > 1 {sub(/^# ?/, ""); print}' "$0"; exit 0 ;;  # leading comment block
        -*) echo "unknown option $a" >&2; exit 2 ;;
        *) ARGS+=("$a") ;;
    esac
done
GIVEN_GAME="${ARGS[0]:-}"
GIVEN_LINUX="${ARGS[1]:-}"
[ ${#ARGS[@]} -le 2 ] || { echo "too many arguments: at most the game folder and the Linux binaries folder" >&2; exit 2; }
case "$PUZZLE_MAKER:$GAME_OPT" in
    0:*|1:|1:portal2|1:all) ;;
    *) echo "--puzzle-maker is for Portal 2: drop it or the --game $GAME_OPT" >&2; exit 2 ;;
esac
if [ "$GAME_OPT" = all ] && [ -n "$GIVEN_GAME" ]; then
    echo "paths are for one game: use --game <game> with them" >&2; exit 2
fi

# Per game, from tools/games.py (its installer entry, the table the Natif
# Installer reads too): title, Steam app and depots, Steam folder, the files a
# complete copy and its Linux depot have, build minutes, app and log names, and
# the Steamworks.NET release whose libsteam_api.dylib has the SDK it asks for.
game_info() {  # game id -> TITLE APPID LINUX_DEPOT DATA_DEPOT STEAM_FOLDER MOD DATA_FILES LINUX_FILES ... STEAMWORKS_URL
    local vars
    vars="$(python3 "$REPO/tools/games.py" --shell "$1")" || die "unknown game $1"
    eval "$vars"
    STEAMWORKS_URL="https://github.com/rlabrecque/Steamworks.NET/releases/download/$STEAMWORKS_VERSION/Steamworks.NET_$STEAMWORKS_VERSION.unitypackage"
}
STEAM_SUPPORT="$HOME/Library/Application Support/Steam"
# Metal renderer: libd3d9metal, built from mtld3d's native host branch (zlib).
MTLD3D_URL="https://github.com/kanapitsas/mtld3d.git"
MTLD3D_REV="101f37d3d45c760e95e20176f758b9d309c23885"   # feat/native-host, pinned
# Prebuilt host libraries and libd3d9metal (built from MTLD3D_REV); see
# tools/build_host_deps.sh, whose BUNDLE_VERSION this matches.
HOST_DEPS_NAME="p2-host-deps-1-macos-arm64"
HOST_DEPS_URL="${P2_HOST_DEPS_URL:-https://github.com/kanapitsas/source-recomp/releases/download/host-deps-1/$HOST_DEPS_NAME.tar.xz}"
HOST_DEPS_SHA256="f5454da33fce938b70df1b7523df0a6884eba4249d5c9092898079964bb75b61"

step() { printf '\n==> %s\n' "$*"; }
# One line each (the installer shows the last "error:" line and collects the
# "warning:" lines): the line breaks of multi-line messages become spaces.
oneline() { printf '%s' "$*" | tr -s '\n ' ' '; }
die() { printf 'error: %s\n' "$(oneline "$*")" >&2; exit 1; }
warn() { printf 'warning: %s\n' "$(oneline "$*")" >&2; }
need() { command -v "$1" >/dev/null 2>&1 || die "$1 not found. $2"; }
config_get() {  # [game id] key -> value from config/local.json for that game (tools/localconfig.py), empty if absent
    [ -f "$CONFIG" ] || return 0
    local id=portal2
    [ $# -gt 1 ] && { id="$1"; shift; }
    python3 -c 'import sys; sys.path.insert(0, sys.argv[1]); import localconfig
print(localconfig.game_config(localconfig.load(sys.argv[2]), sys.argv[3]).get(sys.argv[4]) or "")' \
        "$REPO/tools" "$CONFIG" "$id" "$1"
}
find_depot() {  # app id, depot id -> folder written by the Steam console's download_depot
    [ -d "$STEAM_SUPPORT" ] && [ -n "$2" ] || return 0
    find "$STEAM_SUPPORT" -maxdepth 9 -type d -path "*/content/app_$1/depot_$2" 2>/dev/null | head -1
}

# --- Checks ---------------------------------------------------------------
[ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = arm64 ] || die "this builds for Apple silicon: run it on an arm64 Mac"
step "Checking tools"
xcode-select -p >/dev/null 2>&1 || die "Xcode command line tools missing: xcode-select --install"
need python3 "It comes with the Xcode command line tools: xcode-select --install"
python3 -c 'import sys; sys.exit(sys.version_info < (3, 9))' || die "Python 3.9 or newer is required"
INSTALLER_IDS="$(python3 "$REPO/tools/games.py" --installer-ids)" || die "could not read the game table (tools/games.py)"
is_game() { case " $INSTALLER_IDS " in *" $1 "*) return 0 ;; esac; return 1; }
if [ -n "$GAME_OPT" ] && [ "$GAME_OPT" != all ] && ! is_game "$GAME_OPT"; then
    echo "unknown game $GAME_OPT: use --game $(printf '%s' "$INSTALLER_IDS" | sed 's/ /, /g') or all" >&2; exit 2
fi
if [ "$IOS" = 1 ] && [ -n "$GAME_OPT" ] && [ "$GAME_OPT" != all ]; then
    game_info "$GAME_OPT"
    # Mel joins Portal 2's iPhone build (needs): its Mac build, then ./setup.sh --game portal2 --ios.
    [ -z "$NEEDS" ] || { echo "--ios is not yet supported for $TITLE on its own: build it, then ./setup.sh --game $NEEDS --ios" >&2; exit 2; }
fi
need objdump "It comes with the Xcode command line tools: xcode-select --install"
need curl ""
BREW_PREFIX=/opt/homebrew
brew_has_libs() {
    local lib
    for lib in SDL2 freetype fontconfig; do
        ls "$BREW_PREFIX/lib/lib$lib".* >/dev/null 2>&1 || return 1
    done
    [ -f "$BREW_PREFIX/include/SDL2/SDL.h" ] && [ -d "$BREW_PREFIX/include/freetype2" ]
}
case "${P2_HOST_DEPS:-auto}" in
    homebrew) brew_has_libs || die "P2_HOST_DEPS=homebrew but Homebrew lacks them: brew install sdl2 freetype fontconfig"
              HOST_LIBS_FROM=homebrew ;;
    bundle)   HOST_LIBS_FROM=bundle ;;
    auto)     if brew_has_libs; then HOST_LIBS_FROM=homebrew; else HOST_LIBS_FROM=bundle; fi ;;
    *) die "P2_HOST_DEPS must be homebrew or bundle, not $P2_HOST_DEPS" ;;
esac
# Every build tool, checked up front (tools/build_tools.py: the installer's
# "Setting up tools" step): its PATH (the installer's own copies, Homebrew's,
# Rust's), DEVELOPER_DIR when Xcode is installed but not selected.
eval "$(python3 "$REPO/tools/build_tools.py" env)"
if [ $IOS = 1 ] && [ $DETECT = 0 ]; then   # --detect changes nothing: it needs no Xcode
    if ! python3 "$REPO/tools/build_tools.py" check --for device >/dev/null; then
        step "Setting up tools (XcodeGen, CMake, Ninja, Rust: whatever is missing, in one go)"
        python3 "$REPO/tools/build_tools.py" install --for device \
            || die "the tools aren't all there yet (the lines above say what to click), then try again"
        eval "$(python3 "$REPO/tools/build_tools.py" env)"
    fi
    for t in xcodebuild xcodegen cmake ninja; do
        need "$t" "Open the Natif Installer and click Set up tools (or: python3 tools/build_tools.py install)"
    done
    xcrun --sdk iphoneos --show-sdk-path >/dev/null 2>&1 \
        || die "Xcode has no iOS support yet: open Xcode, then Xcode > Settings > Components, and click Get next to iOS"
fi
echo "ok: $(python3 --version), $(objdump --version | head -1)"

# --- Game folders ---------------------------------------------------------
step "Finding the games"
# A path dragged into Terminal comes with "\ " escapes and a trailing space; inside
# quotes they reach us verbatim. Undo that when the path does not exist as typed.
unescape_path() {
    local p="$1"
    while [ "${p% }" != "$p" ]; do p="${p% }"; done
    [ -e "$p" ] || p="$(printf '%s' "$p" | sed 's/\\\(.\)/\1/g')"
    printf '%s' "$p"
}
[ -n "$GIVEN_GAME" ] && GIVEN_GAME="$(unescape_path "$GIVEN_GAME")"
[ -n "$GIVEN_LINUX" ] && GIVEN_LINUX="$(unescape_path "$GIVEN_LINUX")"
# Paths without --game: the mod folder says which game; anything else is Portal 2.
if [ -z "$GAME_OPT" ] && [ -n "$GIVEN_GAME" ]; then
    GAME_OPT=portal2
    for id in $INSTALLER_IDS; do
        [ "$id" != portal2 ] || continue
        game_info "$id"
        if [ -d "$GIVEN_GAME/$MOD" ]; then GAME_OPT="$id"; break; fi
    done
fi

has_all() {  # folder, relative paths -> true when every one exists
    local d="$1" f; shift
    for f in "$@"; do [ -e "$d/$f" ] || return 1; done
}
portal2_data_ready() {
    local item="$1/portal2_dlc2/scripts/editoritems.txt"
    [ -f "$1/portal2/pak01_dir.vpk" ] && [ -d "$1/platform" ] && [ -f "$item" ] && [ -r "$item" ] && [ -s "$item" ]
}
data_ready() {  # game id, folder -> a complete copy of that game (game_info already run for it)
    has_all "$2" "${DATA_FILES[@]}" && { [ "$1" != portal2 ] || portal2_data_ready "$2"; }
}

locate() {  # game id [game folder [Linux folder]] -> FOUND_GAME FOUND_LINUX (empty when not found)
    local c incomplete=""
    game_info "$1"
    FOUND_GAME="${2:-}"; FOUND_LINUX="${3:-}"
    if [ -z "$FOUND_GAME" ]; then
        # The config, Steam's install, the data depot. The marker file, not just
        # the mod folder: Steam can leave a nearly empty folder behind.
        for c in "$(config_get "$1" game_root)" "$STEAM_SUPPORT/steamapps/common/$STEAM_FOLDER" "$(find_depot "$APPID" "$DATA_DEPOT")"; do
            [ -n "$c" ] || continue
            if data_ready "$1" "$c"; then FOUND_GAME="$c"; break; fi
            [ -n "$incomplete" ] || { [ ! -f "$c/${DATA_FILES[0]}" ] || incomplete="$c"; }
        done
        [ -n "$FOUND_GAME" ] || FOUND_GAME="$incomplete"  # keep the path for an actionable error
    fi
    if [ -z "$FOUND_LINUX" ]; then
        for c in "$(config_get "$1" linux_root)" "$(find_depot "$APPID" "$LINUX_DEPOT")"; do
            # Complete depots only, as check_game wants them: a partial one must not stop another game.
            [ -n "$c" ] && has_all "$c" "${LINUX_FILES[@]}" && { FOUND_LINUX="$c"; break; }
        done
    fi
    return 0
}

check_game() {  # game id -> dies unless FOUND_GAME and FOUND_LINUX are that game's
    local f
    game_info "$1"
    [ -n "$FOUND_GAME" ] || die "$TITLE data not found. Install $TITLE with Steam (the current version),
  pass the game folder (the one with $MOD/ and platform/), or fetch it:
  open steam://open/console, type: download_depot $APPID $DATA_DEPOT, wait for
  'Depot download complete', then re-run."
    has_all "$FOUND_GAME" "${DATA_FILES[@]}" \
        || die "$FOUND_GAME does not look like $TITLE (needs ${DATA_FILES[*]}). Install the current Steam version of $TITLE,
  or fetch it: open steam://open/console, type: download_depot $APPID $DATA_DEPOT, then re-run."
    if [ "$1" = portal2 ]; then
        portal2_data_ready "$FOUND_GAME" || die "Portal 2 game data is incomplete: $FOUND_GAME/portal2_dlc2/scripts/editoritems.txt is missing, unreadable or empty.
  An empty placeholder does not help. In Steam: Portal 2 > Properties > Installed Files >
  Verify integrity of game files; or download the complete game data:
  open steam://open/console, type: download_depot 620 621, wait for
  'Depot download complete', then choose depot_621 as the game folder."
    fi
    [ -n "$FOUND_LINUX" ] || die "$TITLE's Linux binaries not found. Fetch them with your Steam account:
  open steam://open/console, type: download_depot $APPID $LINUX_DEPOT   ($LINUX_SIZE), wait for
  'Depot download complete', then re-run (or pass that folder as the second argument)."
    if [ "$1" = portal1 ] && [ -f "$FOUND_LINUX/bin/shaderapidx9.so" ] && [ ! -f "$FOUND_LINUX/bin/shaderapivk.so" ]; then
        die "This looks like an old Portal Linux build (2013: bin/shaderapidx9.so, no bin/shaderapivk.so, a 5 KB hl2_linux).
  Only the current depot 404 is supported. Fetch it with your Steam account:
  open steam://open/console, type: download_depot 400 404   (about 150 MB), wait for
  'Depot download complete', then re-run (or pass that folder as the second argument)."
    fi
    has_all "$FOUND_LINUX" "${LINUX_FILES[@]}" \
        || die "$FOUND_LINUX does not look like depot $LINUX_DEPOT (needs ${LINUX_FILES[*]})"
    # The Miles sound plugins: in the game's bin/linux32 or the depot's.
    for f in ${AUDIO_FILES[@]+"${AUDIO_FILES[@]}"}; do
        [ -f "$FOUND_GAME/bin/linux32/$f" ] || [ -f "$FOUND_LINUX/bin/linux32/$f" ] \
            || warn "$f is missing from bin/linux32/: some audio may be silent"
    done
    return 0
}

# Which games: --game, else every game whose data and Linux binaries are found.
# A game installed in Steam without its Linux binaries is skipped with a
# warning, unless it is the only one found (then the error says what to fetch).
SELECTED=(); GAME_DIRS=(); LINUX_DIRS=(); SKIPPED=()
case "$GAME_OPT" in
    "") WANTED=() ;;
    all) WANTED=($INSTALLER_IDS) ;;
    *) WANTED=("$GAME_OPT") ;;
esac
if [ ${#WANTED[@]} -gt 0 ]; then
    for id in "${WANTED[@]}"; do
        locate "$id" "$GIVEN_GAME" "$GIVEN_LINUX"
        check_game "$id"
        SELECTED+=("$id"); GAME_DIRS+=("$FOUND_GAME"); LINUX_DIRS+=("$FOUND_LINUX")
    done
else
    for id in $INSTALLER_IDS; do
        locate "$id"
        if [ -n "$FOUND_GAME" ] && [ -n "$FOUND_LINUX" ] && data_ready "$id" "$FOUND_GAME"; then
            check_game "$id"
            SELECTED+=("$id"); GAME_DIRS+=("$FOUND_GAME"); LINUX_DIRS+=("$FOUND_LINUX")
        elif [ -n "$FOUND_GAME" ]; then
            SKIPPED+=("$id")
        fi
    done
    if [ ${#SELECTED[@]} -eq 0 ]; then
        id="${SKIPPED[0]:-portal2}"
        locate "$id"
        [ -n "$FOUND_GAME" ] || die "no game found. Install Portal 2, Half-Life 2, Portal, Portal Stories: Mel or Left 4 Dead 2 with Steam and fetch its Linux binaries
  (README.md), or pass the game folder: ./setup.sh \"/path/to/Portal 2\" \"/path/to/depot_661\""
        check_game "$id"   # says what is missing
    fi
fi
for i in "${!SELECTED[@]}"; do
    game_info "${SELECTED[$i]}"
    echo "$TITLE:"
    echo "  game data:      ${GAME_DIRS[$i]}"
    echo "  Linux binaries: ${LINUX_DIRS[$i]}"
done
SKIPPED_NOTE=""
for id in ${SKIPPED[@]+"${SKIPPED[@]}"}; do
    game_info "$id"
    locate "$id"
    if [ "$id" = portal2 ] && ! data_ready portal2 "$FOUND_GAME"; then
        SKIPPED_NOTE="Portal 2 skipped: incomplete game data in $FOUND_GAME (check portal2_dlc2/scripts/editoritems.txt).
  Restore the Steam game data or download_depot 620 621, then re-run."
    else
        SKIPPED_NOTE="$TITLE is installed but its Linux binaries were not found: to add it, type
  download_depot $APPID $LINUX_DEPOT in the Steam console (open steam://open/console), then re-run."
    fi
    warn "$SKIPPED_NOTE"
done
IOS_SELECTED=()
for id in "${SELECTED[@]}"; do
    game_info "$id"
    [ -n "$NEEDS" ] || IOS_SELECTED+=("$id")   # Mel: inside Portal 2's iPhone build
done
if [ $DETECT = 1 ]; then
    if [ $IOS = 1 ] && [ ${#IOS_SELECTED[@]} -gt 0 ]; then
        echo "iPhone apps: ${IOS_SELECTED[*]}"
    fi
    echo "would build: ${SELECTED[*]}"
    exit 0
fi

# --- Steamworks.NET (each game's release in tools/games.py): steam_api.bundle
steam_dylib() {  # game id -> DYLIB, Valve's libsteam_api.dylib for that game
    game_info "$1"
    local default bundle sw got t d p rel
    DYLIB="$(config_get "$1" steam_api_dylib)"
    if [ "$STEAMWORKS_VERSION" = 20.1.0 ]; then   # Portal 2's release (Mel, Left 4 Dead 2): its historical folder
        default="$REPO/../work/downloads/steamworks/steam_api.bundle/Contents/MacOS/libsteam_api.dylib"
        bundle="$DEPS/steam_api.bundle"
    else
        default="$REPO/../work/downloads/steamworks-$STEAMWORKS_VERSION/steam_api.bundle/Contents/MacOS/libsteam_api.dylib"
        bundle="$DEPS/steamworks-$STEAMWORKS_VERSION/steam_api.bundle"
    fi
    [ -z "$DYLIB" ] && [ -f "$default" ] && DYLIB="$default"
    [ -n "$DYLIB" ] && [ -f "$DYLIB" ] && return 0
    DYLIB="$bundle/Contents/MacOS/libsteam_api.dylib"
    [ -f "$DYLIB" ] && return 0
    step "Downloading Steamworks.NET $STEAMWORKS_VERSION (Valve's libsteam_api.dylib for $TITLE)"
    mkdir -p "$DEPS"
    sw="$DEPS/Steamworks.NET_$STEAMWORKS_VERSION.unitypackage"
    [ -f "$sw" ] || curl -fsSL --retry 3 -o "$sw" "$STEAMWORKS_URL"
    got=$(shasum -a 256 "$sw" | cut -d' ' -f1)
    [ "$got" = "$STEAMWORKS_SHA256" ] || die "$sw: SHA-256 $got, expected $STEAMWORKS_SHA256 (delete it and re-run)"
    t=$(mktemp -d)
    tar -xzf "$sw" -C "$t"
    rm -rf "$bundle"
    for d in "$t"/*; do
        [ -f "$d/asset" ] && [ -f "$d/pathname" ] || continue
        p=$(head -1 "$d/pathname")
        case "$p" in
            */Plugins/steam_api.bundle/*)
                rel="${p#*/Plugins/steam_api.bundle/}"
                mkdir -p "$bundle/$(dirname "$rel")"
                cp "$d/asset" "$bundle/$rel" ;;
        esac
    done
    rm -rf "$t"
    [ -f "$DYLIB" ] || die "steam_api.bundle not found in the Steamworks.NET package"
}
DYLIBS=()
for id in "${SELECTED[@]}"; do
    steam_dylib "$id"
    DYLIBS+=("$DYLIB")
done

# --- Linux i386 system libraries (libstdc++, libgcc_s) for Mel and Left 4 Dead 2
# Their Linux binaries link dynamically against libstdc++.so.6 and libgcc_s.so.1
# (system_libs in tools/games.py; Portal 2 links them statically). Extracted from Debian bullseye's gcc-10 i386
# packages (SHA-256 checked) into deps/debian-i386/lib.
DEBIAN_GCC10_URL="https://deb.debian.org/debian/pool/main/g/gcc-10"
DEBIAN_LIBSTDCXX_DEB="libstdc++6_10.2.1-6_i386.deb"
DEBIAN_LIBSTDCXX_SHA256="ef9b1f03163ad32c8f940b9a9e417773ff3d2f08ad2044d9c28fed5b81de3b7e"
DEBIAN_LIBGCC_DEB="libgcc-s1_10.2.1-6_i386.deb"
DEBIAN_LIBGCC_SHA256="0a52edec5d626f8d9acf1998248d276760aff3950f44a308d27d4a2a488fe18d"
SYSTEM_LIB_DIR=""
fetch_debian_i386() {  # game ids that need them
    local c dest deb deb_path sha url got t found spec id candidates=() titles=()
    for id in "$@"; do candidates+=("$(config_get "$id" system_lib_dir)"); game_info "$id"; titles+=("$TITLE"); done
    for c in "${candidates[@]}" "$REPO/../work/downloads/debian-i386/lib" "$DEPS/debian-i386/lib"; do
        if [ -n "$c" ] && [ -f "$c/libstdc++.so.6" ] && [ -f "$c/libgcc_s.so.1" ]; then
            SYSTEM_LIB_DIR="$c"
            return 0
        fi
    done
    step "Downloading Debian bullseye i386 libstdc++.so.6 and libgcc_s.so.1 (for ${titles[*]})"
    need ar "It comes with the Xcode command line tools: xcode-select --install"
    dest="$DEPS/debian-i386/lib"
    mkdir -p "$DEPS/debian-i386" "$dest"
    for spec in \
        "$DEBIAN_LIBSTDCXX_DEB:$DEBIAN_LIBSTDCXX_SHA256:libstdc++.so.6" \
        "$DEBIAN_LIBGCC_DEB:$DEBIAN_LIBGCC_SHA256:libgcc_s.so.1"; do
        deb="${spec%%:*}"; spec="${spec#*:}"; sha="${spec%%:*}"; lib="${spec#*:}"
        url="$DEBIAN_GCC10_URL/$deb"
        deb_path="$(cd "$DEPS/debian-i386" && pwd)/$deb"
        if [ ! -f "$deb_path" ]; then
            curl -fsSL --retry 3 -o "$deb_path.part" "$url" && mv "$deb_path.part" "$deb_path" \
                || { rm -f "$deb_path.part"; die "could not download $url"; }
        fi
        got=$(shasum -a 256 "$deb_path" | cut -d' ' -f1)
        [ "$got" = "$sha" ] || die "$deb_path: SHA-256 $got, expected $sha (delete it and re-run)"
        t=$(mktemp -d)
        (cd "$t" && ar x "$deb_path" && tar -xf data.tar.*)
        found=$(find "$t" -name "$lib" \( -type f -o -type l \) | head -1)
        [ -n "$found" ] || { rm -rf "$t"; die "$lib not found in $deb"; }
        cp -L "$found" "$dest/$lib"
        rm -rf "$t"
    done
    SYSTEM_LIB_DIR="$dest"
}
NEED_SYSTEM_LIBS=()
for id in "${SELECTED[@]}"; do
    game_info "$id"
    [ -z "$SYSTEM_LIBS" ] || NEED_SYSTEM_LIBS+=("$id")
done
[ ${#NEED_SYSTEM_LIBS[@]} -eq 0 ] || fetch_debian_i386 "${NEED_SYSTEM_LIBS[@]}"

# --- Prebuilt host libraries (when Homebrew's or Rust are missing) --------
HOST_DEPS="$DEPS/$HOST_DEPS_NAME"
METAL_WANTED=1
[ -n "${P2_NO_METAL:-}" ] && [ "$P2_NO_METAL" != 0 ] && METAL_WANTED=0
host_deps_complete() {
    local f
    for f in manifest.json etc/fonts/fonts.conf include/SDL2/SDL.h include/freetype2/ft2build.h \
             lib/libSDL2.dylib lib/libSDL3.dylib lib/libfreetype.dylib lib/libfontconfig.dylib lib/libd3d9metal.dylib; do
        [ -e "$HOST_DEPS/$f" ] || return 1
    done
}
fetch_host_deps() {
    host_deps_complete && return 0
    step "Downloading prebuilt host libraries ($HOST_DEPS_NAME, about 4 MB)"
    mkdir -p "$DEPS"
    local archive="$DEPS/$HOST_DEPS_NAME.tar.xz" got tmp
    if [ ! -f "$archive" ]; then
        curl -fsSL --retry 3 -o "$archive.part" "$HOST_DEPS_URL" && mv "$archive.part" "$archive" \
            || { rm -f "$archive.part"; return 1; }
    fi
    got=$(shasum -a 256 "$archive" | cut -d' ' -f1)
    [ "$got" = "$HOST_DEPS_SHA256" ] || { warn "$archive: SHA-256 $got, expected $HOST_DEPS_SHA256"; rm -f "$archive"; return 1; }
    tmp=$(mktemp -d "$DEPS/.host-deps.XXXXXX")
    tar -xJf "$archive" -C "$tmp" && [ -f "$tmp/$HOST_DEPS_NAME/manifest.json" ] || { rm -rf "$tmp"; return 1; }
    rm -rf "$HOST_DEPS" && mv "$tmp/$HOST_DEPS_NAME" "$HOST_DEPS" && rm -rf "$tmp" "$archive"
    host_deps_complete || { warn "$HOST_DEPS is incomplete"; rm -rf "$HOST_DEPS"; return 1; }
}
HOST_PREFIX=""   # empty: Homebrew
step "Host libraries (SDL2, FreeType, Fontconfig)"
if [ $HOST_LIBS_FROM = bundle ]; then
    python3 -c 'import platform, sys; sys.exit(tuple(map(int, platform.mac_ver()[0].split(".")[:2])) < (13, 0))' \
        || die "the prebuilt libraries need macOS 13 or newer; on older macOS: brew install sdl2 freetype fontconfig"
    fetch_host_deps || die "could not get the prebuilt host libraries from $HOST_DEPS_URL.
  Check the connection and re-run, or install Homebrew's: brew install sdl2 freetype fontconfig"
    HOST_PREFIX="$HOST_DEPS"
    echo "host libraries: prebuilt ($HOST_DEPS)"
else
    echo "host libraries: Homebrew ($BREW_PREFIX)"
fi
# The bundle's libd3d9metal, when built from the same mtld3d commit as ours.
prebuilt_d3d9metal() {
    fetch_host_deps || return 1
    python3 -c 'import json,sys; sys.exit(json.load(open(sys.argv[1])).get("mtld3d_rev") != sys.argv[2])' \
        "$HOST_DEPS/manifest.json" "$MTLD3D_REV" || return 1
    D3D9METAL="$HOST_DEPS/lib/libd3d9metal.dylib"
}

# --- Metal renderer: libd3d9metal (optional) -----------------------------
# Built from a pinned commit of mtld3d's native host branch, or prebuilt from
# the same commit without Rust. Any problem here only costs the Metal renderer:
# the game then uses OpenGL.
D3D9METAL=""
D3D9METAL_IOS=""
METAL_SRC="$DEPS/mtld3d-native"        # owned by setup.sh (marker file); never a checkout of yours
fetch_mtld3d() {
    if [ -e "$METAL_SRC" ] && [ ! -f "$METAL_SRC/.p2-setup" ]; then
        warn "$METAL_SRC exists and was not made by setup.sh: leaving it alone"; return 1
    fi
    [ "$(git -C "$METAL_SRC" rev-parse HEAD 2>/dev/null)" = "$MTLD3D_REV" ] && return 0
    mkdir -p "$METAL_SRC" && touch "$METAL_SRC/.p2-setup" && git -C "$METAL_SRC" init -q \
        && git -C "$METAL_SRC" fetch -q --depth 1 "$MTLD3D_URL" "$MTLD3D_REV" \
        && git -C "$METAL_SRC" checkout -q --force FETCH_HEAD
}
build_d3d9metal() {
    if ! command -v cargo >/dev/null 2>&1; then
        if prebuilt_d3d9metal; then
            echo "Metal renderer: prebuilt libd3d9metal (Rust not found)"
        else
            warn "Rust not found and no prebuilt libd3d9metal: the game will use OpenGL. For the
  faster Metal renderer, install Rust (https://rustup.rs) and re-run ./setup.sh."
        fi
        [ $IOS = 1 ] && warn "the iPhone app needs Rust for Metal (https://rustup.rs): it will use OpenGL"
        return 0
    fi
    step "Building libd3d9metal for the Metal renderer (a few minutes the first time)"
    fetch_mtld3d || { warn "could not get mtld3d from $MTLD3D_URL: the game will use OpenGL"; return 0; }
    local target="$METAL_SRC/native/target" out
    out="$target/production/libd3d9metal.dylib"
    if (cd "$METAL_SRC/native" && CARGO_TARGET_DIR="$target" cargo build -q --profile production) && [ -f "$out" ]; then
        D3D9METAL="$out"
    else
        warn "libd3d9metal did not build (mtld3d needs Rust 1.99 or newer: rustup update): the game will use OpenGL"
        return 0
    fi
    if [ $IOS = 1 ]; then
        out="$target/aarch64-apple-ios/production/libd3d9metal.a"
        if { ! command -v rustup >/dev/null 2>&1 || rustup target add aarch64-apple-ios >/dev/null 2>&1; } \
            && (cd "$METAL_SRC/native" && CARGO_TARGET_DIR="$target" cargo build -q --profile production --target aarch64-apple-ios) \
            && [ -f "$out" ]; then
            D3D9METAL_IOS="$out"
        else
            warn "libd3d9metal for iOS did not build: the iPhone app will use OpenGL"
        fi
    fi
}
METAL_OFF=0
if [ $METAL_WANTED = 0 ]; then
    echo "P2_NO_METAL set: OpenGL renderer only"
    METAL_OFF=1
else
    build_d3d9metal
fi

# --- iOS identity (before writing the config) -----------------------------
TEAM=""
if [ $IOS = 1 ]; then
    step "Finding your Apple development team"
    CERT=$(security find-certificate -a -c "Apple Development" -p 2>/dev/null | awk '/BEGIN/{n++} n==1' || true)
    [ -n "$CERT" ] || die "no Apple Development certificate. In Xcode: Settings > Accounts, add your Apple ID
  (a free one works), select its team, Manage Certificates > + > Apple Development. Then re-run."
    TEAM=$(printf '%s\n' "$CERT" | openssl x509 -noout -subject -nameopt multiline 2>/dev/null \
        | awk -F' = ' '/organizationalUnitName/ {print $2; exit}')
    TEAM="${P2_TEAM_ID:-$TEAM}"
    [ -n "$TEAM" ] || die "could not read your team ID from the certificate; set P2_TEAM_ID"
    echo "team: $TEAM"
fi

# --- config/local.json ----------------------------------------------------
# Shared keys at the top level; Portal 2's game keys too (as in single-game
# configs), another game's in "games" (tools/localconfig.py).
step "Writing config/local.json"
mkdir -p "$REPO/config"
GAME_ARGS=()
for i in "${!SELECTED[@]}"; do GAME_ARGS+=("${SELECTED[$i]}" "${GAME_DIRS[$i]}" "${LINUX_DIRS[$i]}" "${DYLIBS[$i]}"); done
export P2_SETUP_SYSTEM_LIB_DIR="$SYSTEM_LIB_DIR"
python3 - "$REPO/tools" "$CONFIG" "$TEAM" "$D3D9METAL" "$D3D9METAL_IOS" "$METAL_OFF" "$HOST_PREFIX" "${GAME_ARGS[@]}" <<'EOF'
import json, os, sys
from pathlib import Path
sys.path.insert(0, sys.argv[1])
import games, localconfig
path, team, metal, metal_ios, metal_off, host_prefix, *game_args = sys.argv[2:]
p = Path(path)
# An older single-game Half-Life 2 config gets its keys moved into games.hl2.
config = localconfig.migrate(json.loads(p.read_text()) if p.exists() else {})
# Host libraries: the prebuilt bundle's prefix, or none for Homebrew (tools/localconfig.py).
if host_prefix: config['host_prefix'] = host_prefix
else: config.pop('host_prefix', None)
# Metal renderer: recorded when built here; P2_NO_METAL=1 removes it; a path
# that still exists (set by hand, or by an earlier run) is otherwise kept, and
# a stale one is dropped (tools/build_ios.py refuses a missing library instead
# of falling back to OpenGL).
def existing(value):
    return bool(value) and Path(str(value)).expanduser().is_file()
if metal_off == '1':
    config.pop('d3d9metal_dylib', None); config.pop('d3d9metal_ios', None)
else:
    if metal: config['d3d9metal_dylib'] = metal
    elif config.get('d3d9metal_dylib') and not existing(config['d3d9metal_dylib']):
        config.pop('d3d9metal_dylib')
    ios = config.get('d3d9metal_ios') if isinstance(config.get('d3d9metal_ios'), dict) else {}
    ios = {target: path for target, path in ios.items() if existing(path)}
    if metal_ios: ios['device'] = metal_ios
    if ios: config['d3d9metal_ios'] = ios
    else: config.pop('d3d9metal_ios', None)
# Each game: its folders, Valve library and Steam ids (tools/games.py).
for i in range(0, len(game_args), 4):
    name, game, linux, dylib = game_args[i:i + 4]
    known = bool(localconfig.game_config(config, name).get('game_root'))
    sections = config.setdefault('games', {})
    if name == games.DEFAULT and name not in sections:
        target = config   # Portal 2: the top-level keys, unchanged since the first release
    else:
        target = sections.setdefault(name, {})
    target.update(game_root=os.path.normpath(game), linux_root=os.path.normpath(linux),
                  steam_api_dylib=os.path.normpath(dylib))
    sys_lib_dir = os.environ.get('P2_SETUP_SYSTEM_LIB_DIR')
    if sys_lib_dir and games.GAMES[name].get('system_libs'):
        target['system_lib_dir'] = os.path.normpath(sys_lib_dir)
    target.setdefault('steam_appid', games.GAMES[name]['steam_appid'])
    target.setdefault('depot_id', games.GAMES[name]['depot_id'])
    if not known:   # each game its own folder (tools/localconfig.py)
        target.setdefault('user_dir', str(localconfig.support_dir({'game': name}) / 'userdata'))
    if team and games.GAMES[name].get('ios', True) and not localconfig.game_config(config, name).get('ios_bundle_id'):
        # The game's iPhone app. Unique to the team: free accounts cannot reuse
        # an identifier taken elsewhere. Portal 2 keeps its identifier; another
        # game gets its own, so that it does not replace Portal 2's app.
        prefix = 'portal2recomp' if name == games.DEFAULT else f'{name}recomp'
        target['ios_bundle_id'] = f'local.{prefix}.{team.lower()}'
# Sections last: runtime/steam.c reads top-level keys only, but older builds
# took the first match in the file.
sections = config.pop('games', None)
if sections: config['games'] = sections
p.write_text(json.dumps(config, indent=2) + '\n')
for key, value in config.items():
    print(f'{key}: {value}')
EOF

# --- macOS build ----------------------------------------------------------
cd "$REPO"
APP_DIR="${P2_APP_DIR:-$HOME/Applications}"
for i in "${!SELECTED[@]}"; do
    id=${SELECTED[$i]}
    game_info "$id"
    step "Preparing the Linux modules of $TITLE"
    python3 tools/gameprep.py --game "$id"
    step "Recompiling $TITLE (about $MINUTES minutes the first time, seconds afterwards)"
    python3 tools/build_game.py --game "$id" --release
    app_args=(--game "$id" --dest "$APP_DIR")
    if [ "$id" = portal2 ]; then
        if [ $PUZZLE_MAKER = 0 ]; then
            app_args+=(--no-map-compiler)  # not even ones left by an earlier build: they may be stale
        elif [ ! -f "${LINUX_DIRS[$i]}/bin/linux32/vbsp_linux" ]; then
            warn "vbsp_linux is missing: installing Portal 2 without the Puzzle Maker map compilers"
            app_args+=(--no-map-compiler)
        else
            step "Building the Puzzle Maker map compilers (about 10 minutes the first time)"
            if ! python3 tools/build_puzzlemaker.py; then
                warn "Puzzle Maker map compilers did not build: continuing with the Portal 2 installation"
                app_args+=(--no-map-compiler)
            fi
        fi
    fi
    step "Creating $APP_DIR/$APP_NAME.app"
    python3 tools/make_app.py "${app_args[@]}"
done

# --- iOS ------------------------------------------------------------------
if [ $IOS = 1 ] && [ ${#IOS_SELECTED[@]} -gt 0 ]; then
    IOS_DEPS="$DEPS/ios-deps"
    [ -f "$REPO/../work/ios-deps/device/lib/libgl4es.a" ] && [ -z "${P2_DEPS:-}" ] && IOS_DEPS="$REPO/../work/ios-deps"
    if [ ! -f "$IOS_DEPS/device/lib/libgl4es.a" ]; then
        step "Building SDL2, FreeType and gl4es for iOS (a few minutes)"
        P2_IOS_DEPS="$IOS_DEPS" ios/deps/build.sh device
    fi
    # P2_DEVICE picks the device; else a ready iPhone, then a ready iPad
    # (tools/devices.py pick). The first paired device in devicectl's list
    # could be an Apple TV, which got the profile step and the install.
    DEVICE=${P2_DEVICE:-}
    [ -n "$DEVICE" ] || DEVICE=$(python3 tools/devices.py pick 2>/dev/null || true)
    [ -n "$DEVICE" ] || warn "no iPhone connected: a free account needs one registered for its first profile"
    # One identifier per game, or installing one app replaces the other (and
    # its data marker would skip the second copy): P2_BUNDLE_ID applies to all.
    # Round 19: every game lives in the one app (Portal 2's, tools/games.py
    # APP_NAME), each with its data in Documents/<game>. A game whose host isn't
    # set up on this Mac (Half-Life 2 alone) still gets its own app.
    host_of() {
        local host; host=$(python3 tools/ios_game.py --game "$1" --target device | sed -n "s/^HOST='\{0,1\}\([a-z0-9]*\).*/\1/p")
        if [ -n "$host" ] && [ -n "$(python3 tools/ios_game.py --game "$host" --target device | sed -n "s/^GAME_ROOT=//p" | tr -d "'")" ]
        then echo "$host"; else echo "$1"; fi
    }
    IOS_HOSTS=()
    for id in "${IOS_SELECTED[@]}"; do
        h=$(host_of "$id")
        case " ${IOS_HOSTS[*]-} " in *" $h "*) ;; *) IOS_HOSTS+=("$h") ;; esac
    done
    IOS_IDS=$(for id in "${IOS_HOSTS[@]}"; do python3 tools/ios_bundle_id.py --game "$id"; done)
    [ "$(printf '%s\n' "$IOS_IDS" | sort -u | wc -l)" -eq ${#IOS_HOSTS[@]} ] ||
        die "the iPhone apps of ${IOS_HOSTS[*]} would share the identifier $(printf '%s\n' "$IOS_IDS" | sort | uniq -d | head -1)
  (P2_BUNDLE_ID, or the same ios_bundle_id in config/local.json): unset P2_BUNDLE_ID, give each game its own
  ios_bundle_id, or build one game with --game."
    IOS_APPS=()
    for id in "${IOS_HOSTS[@]}"; do
        game_info "$id"
        BUNDLE_ID=$(python3 tools/ios_bundle_id.py --game "$id")
        if [ "$id" = mel ]; then
            # Portal Stories: Mel (phase 1): signed with the team's wildcard profile
            # (ios/signing/sign.py), so no App ID is registered for it on the account
            # (docs/fork/MEL_IOS_PLAN.md). The wildcard has no increased memory limit.
            step "$TITLE: signing with the team's wildcard profile (no new App ID)"
        else
        step "Getting a provisioning profile for $BUNDLE_ID ($TITLE)"
        # iCloud saves (Portal 2): "ios_icloud": true in config/local.json asks Apple for
        # iCloud Documents on this identifier (ios/signing/project-icloud.yml).
        SPEC=project.yml
        case "$id:$(config_get ios_icloud)" in portal2:True|portal2:true|portal2:1) SPEC=project-icloud.yml ;; esac
        # For the connected device by UDID: a generic iOS destination left the
        # iCloud container out of the profile (round 25: "doesn't match the
        # entitlements file's values for ... icloud-container-identifiers").
        DEST=generic/platform=iOS; REG=()
        [ -n "$DEVICE" ] && DEST="platform=iOS,id=$DEVICE" && REG=(-allowProvisioningDeviceRegistration)
        PROFILE_LOG="$REPO/build/ios-profile.log"; mkdir -p "$REPO/build"
        if ! (cd ios/signing && P2_BUNDLE_ID="$BUNDLE_ID" P2_TEAM_ID="$TEAM" xcodegen -q --spec "$SPEC" &&
            xcodebuild -project P2Signing.xcodeproj -scheme Portal2 -destination "$DEST" \
                -allowProvisioningUpdates ${REG[@]+"${REG[@]}"} DEVELOPMENT_TEAM="$TEAM" build >"$PROFILE_LOG" 2>&1); then
            grep -E ' error: |^error: ' "$PROFILE_LOG" | sed 's/^/  /' | head -5
            die "Xcode could not create a profile (the lines above; full log: $PROFILE_LOG). Connect and unlock
  the iPhone, enable Developer Mode (Settings > Privacy & Security), trust this Mac, then re-run."
        fi
        fi
        step "Building the $TITLE iPhone app (about 10 minutes the first time)"
        python3 tools/build_ios.py --game "$id" --target device --release --deps "$IOS_DEPS/device"
        OWN=; [ "$id" != portal2 ] && OWN=--standalone   # a game in IOS_HOSTS other than Portal 2 has its own app
        IOS_APPS+=("$(eval "$(python3 tools/ios_game.py --game "$id" --target device $OWN)"; echo "$IOS_APP")")
        if [ -n "$DEVICE" ]; then
            step "Installing $TITLE on the iPhone"
            P2_STANDALONE=${OWN:+1} P2_DEVICE="$DEVICE" tools/ios_device.sh --game "$id" install
        fi
    done
    # Each game's data, into the app that holds it (its own app: P2_STANDALONE).
    for id in "${IOS_SELECTED[@]}"; do
        [ -n "$DEVICE" ] || break
        game_info "$id"
        STANDALONE=; [ "$(host_of "$id")" = "$id" ] && [ "$id" != portal2 ] && STANDALONE=1
        # The marker lives on the iPhone: a reinstalled app has an empty container.
        # Older Portal 2 setups only left a local marker; trust it if the game folder is there.
        OLD_MARK="build/ios/device/data-copied-$DEVICE"
        SIZE="$DEVICE_SIZE"
        if P2_STANDALONE=$STANDALONE P2_DEVICE="$DEVICE" tools/ios_device.sh --game "$id" has-data; then :
        elif [ "$id" = portal2 ] && [ -f "$OLD_MARK" ] &&
             ! P2_DEVICE="$DEVICE" tools/ios_device.sh --game "$id" has-mark &&
             P2_DEVICE="$DEVICE" tools/ios_device.sh --game "$id" has-game; then :
        else
            step "Copying the $TITLE data to the iPhone ($SIZE; long, once)"
            P2_STANDALONE=$STANDALONE P2_DEVICE="$DEVICE" tools/ios_device.sh --game "$id" data
            # What was copied, so the installer can offer an Update later (tools/device_updates.py).
            P2_STANDALONE=$STANDALONE P2_DEVICE="$DEVICE" tools/ios_device.sh --game "$id" manifest ||
                warn "couldn't leave the update manifest on the device: the installer will copy $TITLE again to update it"
        fi
    done
fi

D3D9METAL="${D3D9METAL:-$(config_get d3d9metal_dylib)}"
if [ -n "$D3D9METAL" ]; then RENDERER="Metal (libd3d9metal); P2_RENDERER=opengl forces OpenGL"
elif [ $METAL_OFF = 1 ]; then RENDERER="OpenGL (P2_NO_METAL set)"
else RENDERER="OpenGL (libd3d9metal unavailable: see the warning above)"; fi
printf '\nDone:\n'
for id in "${SELECTED[@]}"; do
    game_info "$id"
    SAVES_DIR="$(config_get "$id" user_dir)"
    SAVES_DIR="${SAVES_DIR:-$HOME/Library/Application Support/$APP_NAME/userdata}"
    cat <<EOF
- $APP_DIR/$APP_NAME.app
  Start Steam and sign in with the account that owns $TITLE, then open the app
  (it starts Steam itself if needed). Log: ~/Library/Logs/$LOG_NAME
  Saves and settings: $SAVES_DIR
EOF
done
[ -n "$SKIPPED_NOTE" ] && printf -- '- Not built: %s\n' "$SKIPPED_NOTE"
cat <<EOF
- Renderer: ${RENDERER}
- Each app carries its own copy of the game binary, libraries and prepared
  modules; after a rebuild, run python3 tools/make_app.py [--game hl2|portal1|mel|l4d2] to update it.
  The game files stay where Steam put them.
- Keep the builds to yourself: they are made from Valve's games.
- To remove the apps (and, if you confirm, the saves): ./uninstall.sh
EOF
if [ $IOS = 1 ] && [ ${#IOS_SELECTED[@]} -gt 0 ]; then
    # Same lookup as tools/build_ios.py: the library it linked, if any.
    METAL_IOS_LIB=$(python3 -c 'import json,sys; e=json.load(open(sys.argv[1])).get("d3d9metal_ios"); print(e.get("device") or "" if isinstance(e, dict) else "")' "$CONFIG")
    if [ -n "$METAL_IOS_LIB" ]; then IOS_RENDERER="Metal (libd3d9metal)"
    elif [ $METAL_OFF = 1 ]; then IOS_RENDERER="OpenGL (P2_NO_METAL set)"
    elif ! command -v cargo >/dev/null 2>&1; then IOS_RENDERER="OpenGL (install Rust from https://rustup.rs and re-run for Metal)"
    elif ! command -v rustup >/dev/null 2>&1; then IOS_RENDERER="OpenGL (Metal needs Rust from https://rustup.rs: Homebrew's rust has no iOS target)"
    else IOS_RENDERER="OpenGL (libd3d9metal for iOS did not build: see the warning above)"; fi
    echo "- iPhone renderer: ${IOS_RENDERER}"
    for i in "${!IOS_SELECTED[@]}"; do
        game_info "${IOS_SELECTED[$i]}"
        if [ -n "$DEVICE" ]; then
            cat <<EOF
- iPhone: on the phone, Settings > General > VPN & Device Management, trust your
  developer account, then open $TITLE. Options are in Settings > $TITLE.
EOF
        else
            echo "- $TITLE iPhone app built in ${IOS_APPS[$i]}; connect the phone and run:"
            echo "  tools/ios_device.sh --game ${IOS_SELECTED[$i]} install && tools/ios_device.sh --game ${IOS_SELECTED[$i]} data"
        fi
    done
    echo "- A free account's profile lasts 7 days: re-run ./setup.sh --ios to renew it."
fi
