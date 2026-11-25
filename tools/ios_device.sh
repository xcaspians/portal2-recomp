#!/bin/bash
# The game on a real iOS device, over the cable (xcrun devicectl).
#   tools/ios_device.sh [--game portal2|hl2] <command> ...
# Game: --game, else P2_GAME, else config/local.json (tools/games.py selected);
# its app is <ios_dir>/device/<ios_app> (build/ios/device/Portal2.app,
# build/hl2/ios/device/HalfLife2.app) and logs land in <ios_dir>/device.
#   tools/ios_device.sh probe [VAR=value...]    probe app (P2_PROBE_CLIP=1: near-plane clipping)
#   tools/ios_device.sh install                  install the game's app
#   tools/ios_device.sh data                     copy the data into Documents (prep, game, linux)
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
#   tools/ios_device.sh mel data push [--dry-run]   Portal Stories: Mel's data into the Portal app (Documents/mel;
#                                                free space checked; tools/ios_data_plan.py)
#   tools/ios_device.sh mel saves migrate [--dry-run]  Mel's saves from the old standalone Mel app into the Portal
#                                                app, through the Mac, checked file by file; nothing deleted
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
         "saves migrate") set -- mel-saves-migrate "${@:4}" ;;
         *) echo "usage: $0 mel data push [--dry-run] | mel saves migrate [--dry-run]"; exit 2 ;;
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
[ -n "$DEVICE" ] || [ "${1:-} ${2:-}" = "mel-data-push --dry-run" ] ||
  { echo "aucun appareil connecté (câble, appareil déverrouillé, mode développeur)"; exit 1; }
dc() {  # sous-commande de trois mots, --device, puis le reste (options et arguments)
  xcrun devicectl "${@:1:3}" --device "$DEVICE" "${@:4}"
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
  mark_data incomplete
  copy_to "$APP_ID" "$PREP" "$DATA_ROOT/prep"
  copy_to "$APP_ID" "$LINUX_ROOT" "$DATA_ROOT/linux"
  for d in $IOS_DATA; do
    [ -e "$GAME_ROOT/$d" ] && copy_to "$APP_ID" "$GAME_ROOT/$d" "$DATA_ROOT/game/$d"
  done
  mark_data complete
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
  # Toutes les données de Mel (~9,3 Go), sans les restes du joueur dans son
  # dossier de jeu (SAVE, captures, journaux : tools/ios_data_plan.py).
  out=$(python3 tools/ios_data_plan.py --game-root "$GAME_ROOT" --linux-root "$LINUX_ROOT" --prep "$PREP" \
        --folders "$IOS_DATA" --dest-root "$DATA_ROOT") || exit 2
  total=$(awk -F'\t' '$1 == "total" {print $2}' <<< "$out")
  awk -F'\t' '$1 == "copy" {printf "  %-55s %6.0f MB\n", $3, $4 / 1048576}' <<< "$out"
  echo "total: $((total / 1048576)) MB, to the app $APP_ID"
  [ "${2:-}" = --dry-run ] && { echo "dry run: nothing copied"; exit 0; }
  need_space "$total" || exit 1
  mark_data incomplete
  while IFS=$'\t' read -r op from to size; do   # </dev/null : devicectl ne doit pas lire la liste
    [ "$op" = copy ] || continue
    echo "$to"
    copy_to "$APP_ID" "$from" "$to" >/dev/null </dev/null
  done <<< "$out"
  mark_data complete
  echo "Mel's data is on the device. Open the app, or: tools/ios_device.sh --game mel run"
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
