#!/bin/bash
# Apple TV round 1 probe (round 23): builds ios/tvprobe with Xcode's automatic
# signing (the paid team), installs it on the Apple TV, runs it and fetches its
# results (Library/Caches/tvprobe.txt).
#   tools/tvos_probe.sh [build|install|run|all]   (default all)
# Device: P2_TV_DEVICE (UDID or name), else the first paired Apple TV.
# Team: see TEAM below. Identifier: P2_TVPROBE_ID, else
# local.sauce-tvprobe.<team>.
# P2_PROBE_* variables are passed to the probe (ios/tvprobe/main.m).
set -euo pipefail
cd "$(dirname "$0")/.."
# The team: P2_TEAM, else config/local.json "team", else the suffix of Sauce's
# bundle ID (local.portal2recomp.<team>, written by setup.sh).
TEAM=${P2_TEAM:-$(python3 -c 'import json; c = json.load(open("config/local.json"))
print(c.get("team") or (c.get("ios_bundle_id") or "").rsplit(".", 1)[-1].upper())' 2>/dev/null)}
[ -n "$TEAM" ] || { echo "no team: set P2_TEAM or \"team\" in config/local.json"; exit 2; }
export P2_TVPROBE_ID=${P2_TVPROBE_ID:-local.sauce-tvprobe.$(echo "$TEAM" | tr 'A-Z' 'a-z')}
DEVICE=${P2_TV_DEVICE:-$(xcrun devicectl list devices 2>/dev/null |
  awk '/AppleTV/ && /available|connected/ {for (i = 1; i <= NF; ++i) if ($i ~ /^[0-9A-F]{8}-[0-9A-F]{16}$/) {print $i; exit}}')}
[ -n "$DEVICE" ] || { echo "no Apple TV found: pair it (Settings > Remotes and Devices > Remote App and Devices)"; exit 1; }
OUT=build/tvprobe
APP="$OUT/Build/Products/Release-appletvos/SauceTVProbe.app"
step=${1:-all}

if [ "$step" = build ] || [ "$step" = all ]; then
  (cd ios/tvprobe && xcodegen -q)
  xcodebuild -quiet -project ios/tvprobe/SauceTVProbe.xcodeproj -scheme SauceTVProbe -configuration Release \
    -destination "id=$DEVICE" -derivedDataPath "$OUT" -allowProvisioningUpdates -allowProvisioningDeviceRegistration DEVELOPMENT_TEAM="$TEAM" build
  echo "built: $APP"
fi
if [ "$step" = install ] || [ "$step" = all ]; then
  xcrun devicectl device install app --device "$DEVICE" "$APP"
fi
if [ "$step" = run ] || [ "$step" = all ]; then
  env_json=$(python3 -c 'import json, os; print(json.dumps({k: v for k, v in os.environ.items() if k.startswith("P2_PROBE_")}))')
  xcrun devicectl device process launch --device "$DEVICE" --terminate-existing \
    --environment-variables "$env_json" "$P2_TVPROBE_ID" >/dev/null
  # The probe finishes (or is stopped by tvOS at its memory limit) within a minute or two.
  for i in $(seq 1 40); do
    sleep 5
    xcrun devicectl device copy from --device "$DEVICE" --domain-type appDataContainer \
      --domain-identifier "$P2_TVPROBE_ID" --source Library/Caches/tvprobe.txt --destination "$OUT/tvprobe.txt" \
      >/dev/null 2>&1 || continue
    grep -q '^done$\|memory result' "$OUT/tvprobe.txt" && break
  done
  cat "$OUT/tvprobe.txt"
fi
