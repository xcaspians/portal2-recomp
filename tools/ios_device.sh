#!/bin/bash
# The game on a real iOS device, over the cable (xcrun devicectl).
#   tools/ios_device.sh [--game portal2|hl2] <command> ...
# Game: --game, else P2_GAME, else config/local.json (tools/games.py selected);
# its app is <ios_dir>/device/<ios_app> (build/ios/device/Portal2.app,
# build/hl2/ios/device/HalfLife2.app) and logs land in <ios_dir>/device.
#   tools/ios_device.sh probe [VAR=value...]    probe app (P2_PROBE_CLIP=1: near-plane clipping)
#   tools/ios_device.sh install                  install the game's app
#   tools/ios_device.sh data [--only "DEST..."]  copy the data into Documents (prep, game, linux); --only: just
#                                                these destinations (tools/device_updates.py changed), for Update
#   tools/ios_device.sh manifest                 leave the Mac's signature of the data next to it on the device
#                                                (<data root>/.p2-data-manifest.json, tools/device_updates.py)
#   tools/ios_device.sh manifest-get FILE        fetch that manifest (exit 1 when there is none)
#   tools/ios_device.sh saves-backup DIR         every game's saves and settings (Documents/*-userdata) into DIR
#   tools/ios_device.sh has-data                 exit 0 when the data on the device is complete
#   tools/ios_device.sh has-mark                 exit 0 when a copy state marker exists (used by setup.sh)
#   tools/ios_device.sh has-game                 exit 0 when Documents/game/<mod> exists (older copies)
#   tools/ios_device.sh prep                     copy only Documents/prep again (after gameprep: ~80 MB)
#   tools/ios_device.sh run [seconds] [VAR=value...]   launch (default 180 s), then fetch Documents/game.log
#   tools/ios_device.sh backtap                  fetch the back-tap dataset (Documents/backtap/dataset.csv)
#   tools/ios_device.sh mods push <map.bsp | folder> [portal2_dlcN]   a map or an addon onto the device
#                                                (tools/mods_dest.py; docs/mods.md)
#   tools/ios_device.sh workshop push [--dry-run]   the Mac's Workshop maps onto the device, as Community
#                                                Maps keeps them (tools/workshop_push.py; docs/mods.md)
#   tools/ios_device.sh mel data push [--dry-run] [--full]   Portal Stories: Mel's data into the Portal app
#                                                (Documents/mel; free space checked; tools/ios_data_plan.py): the slim
#                                                pack, which reads Portal 2's archives (tools/mel_slim.py), or --full
#   tools/ios_device.sh mel slim switch [--dry-run]   a device with Mel's full copy goes slim: the slim pack's
#                                                extra files, a boot test (Mel's menu, then a save or the first
#                                                map), and only then Mel's own copies of Portal 2's archives removed
#   tools/ios_device.sh mel saves migrate [--dry-run]  Mel's saves from the old standalone Mel app into the Portal
#                                                app, through the Mac, checked file by file; nothing deleted
#   tools/ios_device.sh --game hl2|l4d2 migrate [--dry-run] [--force]   a game's saves and settings from its former
#                                                standalone app into the one app (round 19), through the Mac, checked
#                                                file by file; nothing deleted in the old app
#   tools/ios_device.sh space [bytes]            the device's free space (the app reports it: P2_DEVICE_REPORT=1)
#   tools/ios_device.sh saves status|pull|push [--force] [--dry-run]   saves between the Mac and the iPhone
#                                                (tools/save_sync.py; docs/usage.md#saves-between-mac-and-iphone-portal-2; Portal 2 only)
# Device: P2_DEVICE (UDID or name), else the first connected iPhone/iPad.
set -euo pipefail
cd "$(dirname "$0")/.."
GAME_OPT=()
case "${1:-}" in
  mel) GAME_OPT=(--game mel)
       case "${2:-} ${3:-}" in
         "data push") set -- mel-data-push "${@:4}" ;;
         "slim switch") set -- mel-slim-switch "${@:4}" ;;
         "saves migrate") set -- mel-saves-migrate "${@:4}" ;;
         *) echo "usage: $0 mel data push [--dry-run] [--full] | mel slim switch [--dry-run] | mel saves migrate [--dry-run]"; exit 2 ;;
       esac ;;
  --game) GAME_OPT=(--game "${2:?--game needs a value}"); shift 2 ;;
  --game=*) GAME_OPT=(--game "${1#--game=}"); shift ;;
esac
# Paths and identifiers of the game (tools/ios_game.py): GAME_ROOT, LINUX_ROOT,
# PREP, IOS_DIR, IOS_APP, APP_ID, PROBE_ID, MOD, IOS_DATA.
# P2_STANDALONE=1: a hosted game's own former app (the standalone Mel app).
VARS=$(python3 tools/ios_game.py ${GAME_OPT[@]+"${GAME_OPT[@]}"} --target device ${P2_STANDALONE:+--standalone})
eval "$VARS"
DEVICE=${P2_DEVICE:-$(xcrun devicectl list devices 2>/dev/null |
  awk '/physical/ && / connected|available \(paired\)/ {for (i = 1; i <= NF; ++i) if ($i ~ /^[0-9A-F]{8}-[0-9A-F]{16}$/) {print $i; exit}}')}
# Un essai à blanc de « mel data push » se passe d'appareil.
[ -n "$DEVICE" ] || [[ "${1:-} ${*:2}" == "mel-data-push "*--dry-run* ]] ||
  { echo "aucun appareil connecté (câble, appareil déverrouillé, mode développeur)"; exit 1; }
dc() {  # sous-commande de trois mots, --device, puis le reste (options et arguments)
  xcrun devicectl "${@:1:3}" --device "$DEVICE" "${@:4}"
}
wait_unlocked() {  # 0 once the device reports no passcode needed (up to a minute)
  local i state; state=$(mktemp)
  for i in 1 2 3 4 5 6 7 8 9 10 11 12; do
    dc device info lockState --json-output "$state" >/dev/null 2>&1 &&
      python3 -c 'import json,sys; r=json.load(open(sys.argv[1]))["result"]; sys.exit(0 if r.get("passcodeRequired") is False else 1)' "$state" &&
      { rm -f "$state"; return 0; }
    [ $i = 1 ] && echo "waiting for the device to be unlocked..."
    sleep 5
  done
  rm -f "$state"; return 1
}
copy_journal() {  # what tools/device_copy.py already sent to this device's app (removed once complete)
  echo "$IOS_DIR/copy-journal-$(printf %s "$DEVICE-$APP_ID-$DATA_ROOT" | tr -c 'A-Za-z0-9.-' _).json"
}
env_json() {  # VAR=valeur... -> {"VAR":"valeur",...}
  python3 -c 'import json,sys; print(json.dumps(dict(a.split("=",1) for a in sys.argv[1:])))' "$@"
}
DATA_MARK=$DATA_ROOT/.p2-data-state  # « incomplete » pendant « data », « complete » à la fin (Documents/mel pour Mel)
mark_data() {  # état écrit sur l'appareil (devicectl ne sait pas supprimer un fichier)
  local mark; mark=$(mktemp); echo "$1" > "$mark"
  copy_to "$APP_ID" "$mark" "$DATA_MARK" >/dev/null; rm -f "$mark"
}
has_file() {  # chemin relatif au conteneur : code 0 s'il existe sur l'appareil
  dc device info files --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --subdirectory "$(dirname "$1")" --no-recurse 2>/dev/null |
    awk -v n="$(basename "$1")" '$1 == n {found = 1} END {exit !found}'
}
copy_to() {  # source destination (relative au conteneur de données)
  dc device copy to --domain-type appDataContainer --domain-identifier "$1" --source "$2" --destination "$3"
}
list_saves() {  # fichier JSON : liste horodatée de Documents/portal2-userdata/portal2 (sauvegardes)
  dc device info files --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --subdirectory Documents/portal2-userdata/portal2 --json-output "$1" >/dev/null 2>&1 || {
    echo "cannot list Documents/portal2-userdata/portal2 on the device: is it unlocked, and was the game launched once?"
    exit 1
  }
}
# Free space, model and memory, written by the app itself (ios/main_ios.c,
# P2_DEVICE_REPORT=1: it writes Documents/device-report.txt and quits at once;
# devicectl only knows the total capacity). File path in $1.
device_report() {
  dc device process launch --terminate-existing \
    --environment-variables "$(env_json P2_DEVICE_REPORT=1)" "$APP_ID" >/dev/null 2>&1 || return 1
  local i
  for i in 1 2 3 4 5 6 7 8; do
    sleep 1
    dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" \
      --source Documents/device-report.txt --destination "$1" >/dev/null 2>&1 && grep -q '^free_bytes=' "$1" && return 0
  done
  return 1
}
# Stops with a plain message unless the device has $1 bytes free plus a margin.
need_space() {
  local report; report=$(mktemp)
  device_report "$report" || : > "$report"
  python3 tools/workshop_push.py space --report "$report" --need "$1"; local status=$?
  rm -f "$report"; return $status
}

case "${1:-}" in
probe)
  shift
  dc device install app "$IOS_DIR/probe.app" >/dev/null   # tools/build_ios_probe.sh --game ... device
  dc device process launch --terminate-existing --environment-variables "$(env_json P2_PROBE_LOG=1 "$@")" \
    "$PROBE_ID" >/dev/null
  sleep "${PROBE_SECONDS:-20}"
  mkdir -p "$IOS_DIR"
  dc device copy from --domain-type appDataContainer --domain-identifier "$PROBE_ID" \
    --source Documents/probe.log --destination "$IOS_DIR/probe.log" >/dev/null
  cat "$IOS_DIR/probe.log"
  ;;
install)
  dc device install app "$IOS_APP"
  ;;
data)
  # Dossiers du profil (ios_data) : Portal 2 sans la bande son ni sdk_content
  # (~11,5 Go), Half-Life 2 avec les épisodes et Lost Coast (~10 Go).
  # "copy i/n DEST" lines: the installer's progress bar.
  ONLY=""; [ "${2:-}" = --only ] && ONLY="${3:-}"
  pairs=("$PREP" "$DATA_ROOT/prep" "$LINUX_ROOT" "$DATA_ROOT/linux")
  for d in $IOS_DATA; do [ -e "$GAME_ROOT/$d" ] && pairs+=("$GAME_ROOT/$d" "$DATA_ROOT/game/$d"); done
  todo=()
  for ((i = 0; i < ${#pairs[@]}; i += 2)); do
    if [ -z "$ONLY" ] || python3 -c 'import sys; sys.path.insert(0, "tools"); import device_updates as u; sys.exit(0 if u.keep(sys.argv[1], sys.argv[2].split()) else 1)' "${pairs[i+1]}" "$ONLY"
    then todo+=("${pairs[i]}" "${pairs[i+1]}"); fi
  done
  mark_data incomplete
  # In parts, waiting out a lock or a dropped connection, resumed from the
  # journal after a stop (tools/device_copy.py, round 31).
  for ((i = 0; i < ${#todo[@]}; i += 2)); do printf '%s\t%s\n' "${todo[i]}" "${todo[i+1]}"; done |
    python3 tools/device_copy.py --device "$DEVICE" --app "$APP_ID" --journal "$(copy_journal)"
  mark_data complete
  ;;
manifest)
  m=$(mktemp)
  python3 tools/device_updates.py signature --game "${GAME_OPT[1]:-$(python3 -c 'import sys; sys.path.insert(0, "tools"); import games, localconfig; print(localconfig.game_config(localconfig.load())["game"])')}" --out "$m"
  copy_to "$APP_ID" "$m" "$DATA_ROOT/.p2-data-manifest.json" >/dev/null; rm -f "$m"
  echo "manifest left on the device"
  ;;
manifest-get)
  [ -n "${2:-}" ] || { echo "usage: $0 manifest-get FILE"; exit 2; }
  dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --source "$DATA_ROOT/.p2-data-manifest.json" --destination "$2" >/dev/null 2>&1 && [ -s "$2" ]
  ;;
saves-backup)
  # Before anything replaces the app or its data: every Documents/*-userdata
  # folder (saves, cfg, layouts) onto the Mac. Nothing on the device changes.
  [ -n "${2:-}" ] || { echo "usage: $0 saves-backup DIR"; exit 2; }
  mkdir -p "$2"; listing=$(mktemp)
  if ! dc device info files --domain-type appDataContainer --domain-identifier "$APP_ID" --subdirectory Documents \
       --no-recurse --json-output "$listing" >/dev/null 2>&1; then
    rm -f "$listing"; echo "no saves to back up (the app isn't on the device yet)"; exit 0
  fi
  folders=$(python3 -c '
import json, sys
r = json.load(open(sys.argv[1])).get("result") or {}
for f in r.get("files") or []:
    name = str(f.get("relativePath") or f.get("name") or "").strip("/").split("/")[-1]
    if name.endswith("-userdata"): print(name)' "$listing" | sort -u)
  rm -f "$listing"
  backed=0
  for f in $folders; do
    if dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" --source "Documents/$f" \
         --destination "$2/$f" >/dev/null 2>&1 </dev/null; then
      echo "backed up $f"; backed=$((backed + 1)); continue
    fi
    # devicectl refuses to copy a folder that is empty or gone (error 7000): a
    # Sauce just installed, or reinstalled after being deleted, has nothing to
    # lose there. Only a folder holding files that won't copy stops the install.
    inner=$(mktemp)
    if dc device info files --domain-type appDataContainer --domain-identifier "$APP_ID" --subdirectory "Documents/$f" \
         --json-output "$inner" >/dev/null 2>&1 </dev/null && python3 -c '
import json, sys
files = (json.load(open(sys.argv[1])).get("result") or {}).get("files") or []
# devicectl marks folders with resources.isDirectory (older listings: metadata.type).
def folder(e):
    return bool((e.get("resources") or {}).get("isDirectory")) or "dir" in str((e.get("metadata") or {}).get("type", "")).lower()
sys.exit(0 if any(not folder(e) for e in files) else 1)' "$inner"; then
      rm -f "$inner"; echo "couldn't back up Documents/$f: stopping before anything is replaced"; exit 1
    fi
    rm -f "$inner"; echo "nothing to back up in $f (empty)"
  done
  if [ $backed = 0 ]; then echo "no saves to back up (nothing saved on the device yet)"; else echo "saves backed up in $2"; fi
  ;;
has-data)
  state=$(mktemp)
  dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --source "$DATA_MARK" --destination "$state" >/dev/null 2>&1 && grep -qx complete "$state"
  status=$?; rm -f "$state"; exit $status
  ;;
has-mark)  # un état existe (copie commencée par cette version du script)
  has_file "$DATA_MARK"
  ;;
has-game)  # copies antérieures au marqueur : le dossier du jeu suffit
  has_file "$DATA_ROOT/game/$MOD"
  ;;
prep)
  # Le dossier entier en un appel : chaque fichier remplace son homologue (les
  # adresses des modules suivent $PREP, celui avec lequel l'app a été construite).
  copy_to "$APP_ID" "$PREP" "$DATA_ROOT/prep" >/dev/null
  echo "Documents/prep : $(ls "$PREP" | wc -l | tr -d ' ') fichiers copiés"
  ;;
run)
  shift
  SECONDS_MAX=180
  case "${1:-}" in ''|*=*) ;; *) SECONDS_MAX=$1; shift ;; esac   # seconds are optional: `run P2_RENDERER=opengl`
  dc device process launch --terminate-existing \
    --environment-variables "$(env_json P2_LOG_FILE=Documents/game.log P2_FRAME_LOG=1 P2_GAME_CHOICE="$GAME" "$@")" "$APP_ID"
  sleep "$SECONDS_MAX"
  mkdir -p "$IOS_DIR"
  dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --source Documents/game.log --destination "$IOS_DIR/game.log"
  echo "journal : $IOS_DIR/game.log ($(wc -l < "$IOS_DIR/game.log") lignes)"
  ;;
backtap)
  mkdir -p "$IOS_DIR/backtap"
  dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --source Documents/backtap/dataset.csv --destination "$IOS_DIR/backtap/dataset.csv"
  echo "dataset: $IOS_DIR/backtap/dataset.csv ($(($(wc -l < "$IOS_DIR/backtap/dataset.csv") - 1)) examples)"
  ;;
mods)
  # Cartes dans Documents/game/portal2/maps/custom, extensions dans
  # Documents/game/portal2_dlcN (tools/mods_dest.py : destinations vérifiées).
  [ "${2:-}" = push ] && [ -n "${3:-}" ] || { echo "usage: $0 mods push <map.bsp | folder> [portal2_dlcN]"; exit 2; }
  pairs=$(python3 tools/mods_dest.py "$3" ${4:+"$4"}) || exit 2
  while IFS=$'\t' read -r from to; do   # </dev/null : devicectl ne doit pas lire la liste
    echo "$from -> $to"
    copy_to "$APP_ID" "$from" "$to" >/dev/null </dev/null
  done <<< "$pairs"
  echo "Copied. In the game: pause menu > Touch Controls > Maps & Mods, or a line +map custom/<name> in Documents/launch.args"
  ;;
space)
  report=$(mktemp)
  device_report "$report" || { echo "the app didn't report (installed, device unlocked?)"; exit 1; }
  cat "$report"; rm -f "$report"
  [ -z "${2:-}" ] || need_space "$2"
  ;;
mel-data-push)
  # Mel's data (Documents/mel), without the player's leftovers in its game
  # folder (SAVE, screenshots, logs: tools/ios_data_plan.py). By default the
  # slim pack (~6.8 GB instead of ~9.2): Portal 2's VPK archives are read from
  # Portal 2's own data on the device (tools/mel_slim.py, runtime/paths.c), so
  # Portal 2 must be there first. --full: Mel's whole copy, as before.
  full=0; dry=0; ONLY=""; next_only=0
  for a in "${@:2}"; do
    if [ $next_only = 1 ]; then ONLY="$a"; next_only=0; continue; fi
    case "$a" in --full) full=1 ;; --dry-run) dry=1 ;; --only) next_only=1 ;; esac
  done
  slim_args=()
  if [ $full = 0 ]; then
    python3 tools/mel_slim.py build --mel "$GAME_ROOT" --out build/mel/slim >/dev/null || exit 2
    slim_args=(--shared "portal2 portal2_dlc2" --overlay build/mel/slim)
  fi
  out=$(python3 tools/ios_data_plan.py --game-root "$GAME_ROOT" --linux-root "$LINUX_ROOT" --prep "$PREP" \
        --folders "$IOS_DATA" --dest-root "$DATA_ROOT" ${slim_args[@]+"${slim_args[@]}"}) || exit 2
  total=$(awk -F'\t' '$1 == "total" {print $2}' <<< "$out")
  awk -F'\t' '$1 == "copy" {printf "  %-55s %6.0f MB\n", $3, $4 / 1048576}' <<< "$out" | head -40
  echo "total: $((total / 1048576)) MB ($([ $full = 1 ] && echo "full copy" || echo "slim pack")), to the app $APP_ID"
  [ $dry = 1 ] && { echo "dry run: nothing copied"; exit 0; }
  if [ $full = 0 ] && ! has_file Documents/game/portal2/pak01_dir.vpk; then
    echo "Mel's slim pack reads Portal 2's files, and Portal 2 isn't on this device: copy it first (tools/ios_device.sh data), or use --full"
    exit 1
  fi
  need_space "$total" || exit 1
  mark_data incomplete
  while IFS=$'\t' read -r op from to size; do
    [ "$op" = copy ] || continue
    [ -z "$ONLY" ] || python3 -c 'import sys; sys.path.insert(0, "tools"); import device_updates as u; sys.exit(0 if u.keep(sys.argv[1], sys.argv[2].split()) else 1)' "$to" "$ONLY" || continue
    printf '%s\t%s\n' "$from" "$to"
  done <<< "$out" | python3 tools/device_copy.py --device "$DEVICE" --app "$APP_ID" --journal "$(copy_journal)"
  if [ $full = 0 ]; then   # the app reads Portal 2's archives from now on (ios/main_ios.c)
    m=$(mktemp); echo slim > "$m"; copy_to "$APP_ID" "$m" "$DATA_ROOT/.p2-slim" >/dev/null; rm -f "$m"
  fi
  mark_data complete
  echo "Mel's data is on the device. Open the app, or: tools/ios_device.sh --game mel run"
  ;;
mel-slim-switch)
  # A device holding Mel's full copy goes slim, never losing anything on the way:
  # 1. the slim pack's VPK directories (portal2/pak01_slim.vpk) go next to Mel's
  #    own (harmless to the full copy: only the slim mode reads them);
  # 2. boot test with the slim layout (P2_MEL_SLIM=1): Mel's menu, then its newest
  #    save (or the first map when there is none), checked in the game's log;
  # 3. only then the app removes Mel's own copies of Portal 2's archives
  #    (P2_MEL_SLIM_COMMIT=1: devicectl can't delete) and marks Mel slim.
  python3 tools/mel_slim.py build --mel "$GAME_ROOT" --out build/mel/slim || exit 2
  plan=$(python3 tools/ios_data_plan.py --game-root "$GAME_ROOT" --linux-root "$LINUX_ROOT" --prep "$PREP" \
         --folders "" --dest-root "$DATA_ROOT" --overlay build/mel/slim) || exit 2
  freed=$(python3 tools/mel_slim.py plan --mel "$GAME_ROOT" --json | python3 -c 'import json,sys; print(json.load(sys.stdin)["shared_bytes"])')
  echo "frees about $((freed / 1000000)) MB on the device once confirmed"
  [ "${2:-}" = --dry-run ] && { echo "dry run: nothing copied"; exit 0; }
  has_file "$DATA_ROOT/game/$MOD/gameinfo.txt" || { echo "Mel isn't on this device: use tools/ios_device.sh mel data push"; exit 1; }
  has_file Documents/game/portal2/pak01_dir.vpk || { echo "Portal 2 isn't on this device: Mel can't go slim"; exit 1; }
  while IFS=$'\t' read -r op from to size; do
    case "$to" in */pak01_slim.vpk) ;; *) continue ;; esac   # the slim directories only (not prep or linux)
    copy_to "$APP_ID" "$from" "$to" >/dev/null </dev/null
  done <<< "$plan"
  echo "slim directories copied; boot test (90 s)..."
  # The test needs the iPad unlocked and awake the whole time: locked or asleep,
  # iOS stops the game drawing its menu and the save loads too early (the first
  # attempt on the iPad mini: "Can't load ..., file missing!").
  wait_unlocked || { echo "the device is locked: unlock it, keep it awake, and run this again (nothing was deleted)"; exit 1; }
  save=$(dc device info files --domain-type appDataContainer --domain-identifier "$APP_ID" \
           --subdirectory "Documents/mel-userdata/$MOD/SAVE" 2>/dev/null |
         awk '$1 ~ /\.sav$/ && $1 !~ /^autosave/ {print $1}' | tail -1)
  # From the menu once it's up (P2_IOS_MENU_EXEC, runtime/ios/game_state.c), as a
  # player would: a command-line +load runs before the menu on a slower device.
  # The listing names the save with its Steam ID folder (7656.../1795587607.sav);
  # the engine adds that folder itself, so only the file name is given.
  save=${save##*/}
  load="map sp_a1_tramride"; [ -n "$save" ] && load="load ${save%.sav}"
  mkdir -p "$IOS_DIR"; log="$IOS_DIR/mel-slim-test.log"; before="$IOS_DIR/mel-slim-before.log"
  # -condebug appends to Documents/mel-userdata/portal_stories/console.log across
  # launches: only what this launch adds is judged (its length beforehand).
  : > "$before"
  dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --source "Documents/mel-userdata/$MOD/console.log" --destination "$before" >/dev/null 2>&1 || : > "$before"
  since=$(wc -c < "$before" | tr -d ' ')
  dc device process launch --terminate-existing --environment-variables \
    "$(env_json P2_LOG_FILE=Documents/game.log P2_GAME_CHOICE=mel P2_MEL_SLIM=1 P2_EXTRA_ARGS=-condebug P2_IOS_MENU_EXEC="$load")" "$APP_ID" >/dev/null
  sleep 90
  dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --source "Documents/mel-userdata/$MOD/console.log" --destination "$log" >/dev/null
  dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --source Documents/game.log --destination "$IOS_DIR/mel-slim-game.log" >/dev/null 2>&1 || :
  if grep -aq "app en arrière-plan" "$IOS_DIR/mel-slim-game.log" 2>/dev/null; then
    echo "the device went to sleep or left the app during the test: nothing was deleted; keep it awake and run this again"
    exit 1
  fi
  if ! python3 tools/mel_slim.py boot-ok --log "$log" --since "$since"; then
    echo "boot test failed: Mel's full copy is untouched. Log: $log (from byte $since)"; exit 1
  fi
  dc device process launch --terminate-existing --environment-variables \
    "$(env_json P2_LOG_FILE=Documents/slim-commit.log P2_GAME_CHOICE=mel P2_MEL_SLIM_COMMIT=1)" "$APP_ID" >/dev/null
  sleep 15
  dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --source Documents/slim-commit.log --destination "$IOS_DIR/mel-slim-commit.log" >/dev/null
  grep -a "paquet allégé" "$IOS_DIR/mel-slim-commit.log" || { echo "the app didn't confirm the switch: see $IOS_DIR/mel-slim-commit.log"; exit 1; }
  ;;
mel-saves-migrate)
  # Les sauvegardes de l'ancienne app Mel autonome vers l'app Portal (Mel y est
  # hébergé : mêmes dossiers mel-userdata), en passant par le Mac. Rien n'est
  # supprimé dans l'ancienne app ; chaque fichier est vérifié à l'arrivée.
  OLD_ID=$(python3 tools/ios_bundle_id.py --game mel)
  [ "$OLD_ID" != "$APP_ID" ] || { echo "Mel isn't hosted in the Portal app here (no ios_host)"; exit 2; }
  SAVE_DIR=Documents/mel-userdata/$MOD/SAVE
  work="$IOS_DIR/mel-saves-migration"; rm -rf "$work"; mkdir -p "$work"
  dc device copy from --domain-type appDataContainer --domain-identifier "$OLD_ID" --source "$SAVE_DIR" \
    --destination "$work/SAVE" >/dev/null 2>&1 || { echo "no saves found in the old Mel app ($OLD_ID): nothing to move"; exit 0; }
  n=$(find "$work/SAVE" -type f \( -iname '*.sav' -o -iname '*.tga' \) | wc -l | tr -d ' ')
  echo "$n save files in the old Mel app, kept there:"; (cd "$work/SAVE" && find . -type f | sed 's/^/  /')
  [ "${2:-}" = --dry-run ] && { echo "dry run: nothing copied"; exit 0; }
  copy_to "$APP_ID" "$work/SAVE" "$SAVE_DIR" >/dev/null
  listing="$work/after.json"
  dc device info files --domain-type appDataContainer --domain-identifier "$APP_ID" --subdirectory "$SAVE_DIR" \
    --json-output "$listing" >/dev/null 2>&1 || { echo "couldn't list the saves in the Portal app afterwards"; exit 1; }
  python3 tools/ios_data_plan.py verify --local "$work/SAVE" --listing "$listing" || exit 1
  echo "Mel's saves are in the Portal app. Check them in the game, then remove the old Mel app (Needs Caspian)."
  ;;
migrate)
  # Round 19: Half-Life 2 and Left 4 Dead 2 moved from their own apps into the
  # one app. Their whole saves folder (Documents/<userdata>: SAVE, cfg, video
  # settings) and the old app's preferences go over through the Mac; the app
  # takes the old button layout at the game's next launch
  # (live_settings_ios.m). Nothing is deleted in the old app.
  OLD_ID=$(python3 tools/ios_game.py ${GAME_OPT[@]+"${GAME_OPT[@]}"} --target device --standalone | sed -n "s/^APP_ID=//p" | tr -d "'")
  [ -n "$HOST" ] && [ "$OLD_ID" != "$APP_ID" ] || { echo "$GAME has no former app of its own here (not hosted in the one app)"; exit 2; }
  [ -n "$USERDATA" ] || { echo "$GAME: no saves folder known (runtime/ios/game_ios.c)"; exit 2; }
  dry=0; force=0
  for a in "${@:2}"; do case "$a" in --dry-run) dry=1 ;; --force) force=1 ;; *) echo "usage: $0 --game $GAME migrate [--dry-run] [--force]"; exit 2 ;; esac; done
  work="$IOS_DIR/$GAME-migration"; rm -rf "$work"; mkdir -p "$work"
  dc device copy from --domain-type appDataContainer --domain-identifier "$OLD_ID" --source "Documents/$USERDATA" \
    --destination "$work/$USERDATA" >/dev/null 2>&1 || { echo "nothing to move: no Documents/$USERDATA in the old app ($OLD_ID)"; exit 0; }
  dc device copy from --domain-type appDataContainer --domain-identifier "$OLD_ID" \
    --source "Library/Preferences/$OLD_ID.plist" --destination "$work/$USERDATA/settings-from-old-app.plist" >/dev/null 2>&1 ||
    echo "(no preferences in the old app: its saves only)"
  n=$(find "$work/$USERDATA" -type f | wc -l | tr -d ' ')
  echo "$n files from the old $TITLE app ($OLD_ID), kept there:"; (cd "$work/$USERDATA" && find . -type f | sed 's/^/  /')
  [ $dry = 1 ] && { echo "dry run: nothing copied"; exit 0; }
  # Never over saves the one app already has (a game played there first): --force to merge anyway.
  listing="$work/before.json"
  if [ $force = 0 ] && dc device info files --domain-type appDataContainer --domain-identifier "$APP_ID" \
       --subdirectory "Documents/$USERDATA" --json-output "$listing" >/dev/null 2>&1 &&
     python3 -c 'import json,sys; d=json.load(open(sys.argv[1])); sys.exit(0 if any(f.get("relativePath","").lower().endswith(".sav") for f in (d.get("result") or {}).get("files") or []) else 1)' "$listing"; then
    echo "the one app already has $TITLE saves in Documents/$USERDATA: nothing copied (--force merges, the old app's files win)"; exit 1
  fi
  copy_to "$APP_ID" "$work/$USERDATA" "Documents/$USERDATA" >/dev/null
  listing="$work/after.json"
  dc device info files --domain-type appDataContainer --domain-identifier "$APP_ID" --subdirectory "Documents/$USERDATA" \
    --json-output "$listing" >/dev/null 2>&1 || { echo "couldn't list Documents/$USERDATA in the one app afterwards"; exit 1; }
  python3 tools/ios_data_plan.py verify --local "$work/$USERDATA" --listing "$listing" || exit 1
  echo "$TITLE's saves and settings are in the one app. Check them in the game, then remove the old app (Needs Caspian)."
  ;;
workshop)
  # Documents/portal2-userdata/portal2/maps/workshop/<id>/ : la disposition de
  # Community Maps (workshop_ios.m), avec sa note p2workshop.txt. Rien n'est
  # jamais supprimé sur l'appareil (tools/workshop_push.py).
  [ "${2:-}" = push ] || { echo "usage: $0 workshop push [--dry-run]"; exit 2; }
  [ "$MOD" = portal2 ] || { echo "workshop: Portal 2 only"; exit 2; }
  dry=""; [ "${3:-}" = --dry-run ] && dry=--dry-run
  mkdir -p "$IOS_DIR"
  listing="$IOS_DIR/workshop-listing.json"; : > "$listing"
  if ! dc device info files --domain-type appDataContainer --domain-identifier "$APP_ID" \
       --subdirectory Documents/portal2-userdata/portal2/maps/workshop --json-output "$listing" >/dev/null 2>&1; then
    : > "$listing"   # no workshop folder yet, or no device: tell them apart
    dc device info files --domain-type appDataContainer --domain-identifier "$APP_ID" \
      --subdirectory Documents --no-recurse >/dev/null 2>&1 ||
      { echo "cannot list the app's files on the device: is it unlocked, and is Portal 2 installed?"; exit 1; }
  fi
  stage="$IOS_DIR/workshop-stage"; rm -rf "$stage"; mkdir -p "$stage"
  out=$(python3 tools/workshop_push.py plan --listing "$listing" --stage "$stage" $dry) || exit $?
  total=$(awk -F'\t' '$1 == "total" {print $2}' <<< "$out")
  [ -n "$dry" ] && { echo "dry run: nothing copied"; exit 0; }
  [ "${total:-0}" -gt 0 ] && { need_space "$total" || exit 1; }
  n=0
  while IFS=$'\t' read -r op from to; do   # </dev/null : devicectl ne doit pas lire la liste
    [ "$op" = copy ] || continue
    echo "$to"
    copy_to "$APP_ID" "$from" "$to" >/dev/null </dev/null
    n=$((n + 1))
  done <<< "$out"
  rm -rf "$stage"
  echo "$n copied. In the game: pause menu > Touch Controls > Maps & Mods > Community Maps > Downloaded"
  ;;
saves)
  # tools/save_sync.py knows Portal 2's folders only (portal2-userdata/portal2).
  userdata=""; case "$MOD" in portal2) userdata=portal2-userdata ;; portal) userdata=portal-userdata ;; hl2_complete) userdata=hl2-userdata ;; esac
  [ "$MOD" = portal2 ] || { echo "saves: Portal 2 only for now ($MOD saves are in Documents/$userdata on the device)"; exit 2; }
  action=${2:-status}; shift; shift || true
  case "$action" in status|pull|push) ;; *) echo "saves status|pull|push [--force] [--dry-run]"; exit 2 ;; esac
  if [ "$action" = pull ] && pgrep -x portal2-native >/dev/null && [[ " $* " != *" --force "* ]]; then
    echo "the game is running on the Mac: quit it first (it rewrites its saves)"; exit 1
  fi
  mkdir -p "$IOS_DIR"
  listing="$IOS_DIR/saves-listing.json"   # kept for debugging
  sync=(python3 tools/save_sync.py)
  list_saves "$listing"
  if [ "$action" = status ]; then "${sync[@]}" status --listing "$listing" --device "$DEVICE"; exit; fi
  ops=$("${sync[@]}" plan "$action" --listing "$listing" --device "$DEVICE" "$@")
  case " $* " in *" --dry-run "*) exit 0 ;; esac
  # Copies faites ou non, l'état ne retient que celles arrivées (save_sync.py record),
  # plus les fichiers déjà identiques (référence pour la prochaine comparaison).
  trap 'list_saves "$listing" && "${sync[@]}" record --listing "$listing" --device "$DEVICE"' EXIT
  while IFS=$'\t' read -r op from to; do   # </dev/null : devicectl ne doit pas lire la liste
    case "$op" in
    from) dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" \
            --source "$from" --destination "$to" >/dev/null </dev/null ;;
    to) copy_to "$APP_ID" "$from" "$to" >/dev/null </dev/null ;;
    esac
  done <<< "$ops"
  ;;
*) awk 'NR > 1 && !/^#/ {exit} NR > 1 {sub(/^# ?/, ""); print}' "$0"; exit 2 ;;
esac
