#!/bin/bash
# Builds "Natif Installer.app" (installer/Installer.swift; every game of
# tools/games.py's installer table) with a snapshot of the committed repository inside, and a zip
# for a GitHub release:
#   build/installer/Natif Installer.app
#   build/installer/Natif-Installer-<version>.zip
# The name is set once here (INSTALLER_NAME); the bundle identifier stays the
# first release's.
#   build/installer/Natif-Installer-<version>.dmg   (with --dmg: open it, drag
#   the app to Applications; `make installer-dmg`)
# usage: tools/build_installer.sh [--dmg | --release] [version]   (default: git describe)
#   --release: the DMG of a GitHub release (`make installer-release`). HEAD
#   must be a clean, tagged commit; it stops unless the app and the DMG are
#   signed with a Developer ID, notarized, stapled and accepted by spctl, so a
#   release never ships ad hoc (no Open Anyway step for users).
# Needs only the Xcode command line tools. The snapshot is `git archive HEAD`:
# uncommitted changes are not in it.
#
# Signing: with a "Developer ID Application" certificate in the keychain (or
# P2_SIGN_IDENTITY naming one), the app is signed with the hardened runtime
# and a timestamp; with a notarytool profile too (P2_NOTARY_PROFILE, default
# sauce-notary, made once by `xcrun notarytool store-credentials`), the zip is
# notarized and the app stapled, so Gatekeeper opens it without a warning.
# Without a certificate: ad hoc signature, users click Open Anyway once.
# P2_SIGN_IDENTITY=- forces ad hoc. See docs/usage.md#releases.
set -euo pipefail
REPO="$(cd "$(dirname "$0")/.." && pwd)"
cd "$REPO"
DMG=0
RELEASE=0
case "${1:-}" in
    --dmg) DMG=1; shift ;;
    --release) DMG=1; RELEASE=1; shift ;;
esac
fail() { echo "error: $*" >&2; exit 1; }
if [ $RELEASE = 1 ]; then
    git diff --quiet HEAD -- && [ -z "$(git status --porcelain --untracked-files=no)" ] \
        || fail "--release: commit first (the app embeds git archive HEAD)"
    git describe --tags --exact-match HEAD >/dev/null 2>&1 || fail "--release: tag HEAD first (git tag vX.Y.Z)"
fi
VERSION="${1:-$(git describe --tags --always --dirty)}"
# Bundle versions must be numbers (0.2.0); the full description goes in P2SourceVersion.
NUMERIC=$(printf '%s' "${VERSION#v}" | sed -E 's/^([0-9]+(\.[0-9]+){0,2}).*/\1/')
case "$NUMERIC" in [0-9]*) ;; *) NUMERIC=0.0.0 ;; esac
IDENTITY="${P2_SIGN_IDENTITY:-$(security find-identity -v -p codesigning 2>/dev/null \
    | sed -n 's/.*"\(Developer ID Application: [^"]*\)".*/\1/p' | head -1)}"
IDENTITY="${IDENTITY:--}"
PROFILE="${P2_NOTARY_PROFILE:-sauce-notary}"
if [ $RELEASE = 1 ]; then
    [ "$IDENTITY" != - ] || fail "--release: no Developer ID Application certificate in the keychain (docs/usage.md#releases)"
    xcrun notarytool history --keychain-profile "$PROFILE" >/dev/null 2>&1 \
        || fail "--release: notarytool profile $PROFILE missing or rejected (docs/usage.md#releases)"
fi
OUT="$REPO/build/installer"
INSTALLER_NAME="Natif Installer"
APP="$OUT/$INSTALLER_NAME.app"
git diff --quiet HEAD -- || echo "warning: uncommitted changes are not in the installer's snapshot" >&2

rm -rf "$APP" && mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources"
# Universal (Apple silicon and Intel): the installer opens on any Mac with
# macOS 13. The Mac games themselves need Apple silicon (arm64 programs and
# Valve's arm64 libsteam_api.dylib); the window says so on an Intel Mac.
SLICES=()
for arch in arm64 x86_64; do
    swiftc -O -parse-as-library -swift-version 5 -target $arch-apple-macos13.0 \
        installer/*.swift -o "$OUT/installer-$arch"
    SLICES+=("$OUT/installer-$arch")
done
lipo -create "${SLICES[@]}" -output "$APP/Contents/MacOS/installer"
rm -f "${SLICES[@]}"
# SOURCE_VERSION: the release this source is (Update Sauce compares it; an older installer
# never puts its own older source back over a newer one).
git archive --format=tar.gz --add-virtual-file="SOURCE_VERSION:$VERSION
" -o "$APP/Contents/Resources/source.tar.gz" HEAD
# Its own copy of the process supervisor (round 31): it runs before the source is unpacked,
# to stop what an earlier run left going, whatever version that source is.
cp tools/installer_runs.py "$APP/Contents/Resources/installer_runs.py"
# The games both tabs offer: tools/games.py's table, as the app reads it (Game in installer/Installer.swift).
python3 tools/games.py --installer-json >"$APP/Contents/Resources/games.json"
cat >"$APP/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleName</key><string>$INSTALLER_NAME</string>
  <key>CFBundleDisplayName</key><string>$INSTALLER_NAME</string>
  <key>CFBundleIdentifier</key><string>io.github.kanapitsas.portal2-recomp.installer</string>
  <key>CFBundleExecutable</key><string>installer</string>
  <key>CFBundlePackageType</key><string>APPL</string>
  <key>CFBundleShortVersionString</key><string>$NUMERIC</string>
  <key>CFBundleVersion</key><string>$NUMERIC</string>
  <key>P2SourceVersion</key><string>$VERSION</string>
  <key>LSMinimumSystemVersion</key><string>13.0</string>
  <key>LSArchitecturePriority</key><array><string>arm64</string><string>x86_64</string></array>
  <key>NSHighResolutionCapable</key><true/>
</dict>
</plist>
EOF
ZIP="$OUT/${INSTALLER_NAME// /-}-$VERSION.zip"
DMG_FILE="$OUT/${INSTALLER_NAME// /-}-$VERSION.dmg"
# The DMG: the app and a link to /Applications, to drag it onto.
make_dmg() {
    local stage="$OUT/dmg-stage"
    rm -rf "$stage" "$DMG_FILE" && mkdir -p "$stage"
    ditto "$APP" "$stage/$INSTALLER_NAME.app"
    ln -s /Applications "$stage/Applications"
    hdiutil create -quiet -volname "$INSTALLER_NAME" -srcfolder "$stage" -fs HFS+ -format UDZO -ov "$DMG_FILE"
    rm -rf "$stage"
}
pack() {
    if [ $DMG = 1 ]; then make_dmg; else rm -f "$ZIP" && (cd "$OUT" && ditto -c -k --keepParent "$INSTALLER_NAME.app" "$ZIP"); fi
}
PACKAGE() { [ $DMG = 1 ] && echo "$DMG_FILE" || echo "$ZIP"; }
if [ "$IDENTITY" = - ]; then
    echo "signing: ad hoc (no Developer ID Application certificate): users will need Open Anyway"
    codesign --force -s - "$APP"
    pack
else
    echo "signing: $IDENTITY"
    codesign --force --options runtime --timestamp -s "$IDENTITY" "$APP"
    codesign --verify --strict --verbose=1 "$APP"
    pack
    [ $DMG = 1 ] && codesign --force --timestamp -s "$IDENTITY" "$DMG_FILE"
    if xcrun notarytool history --keychain-profile "$PROFILE" >/dev/null 2>&1; then
        echo "notarizing with profile $PROFILE (a few minutes)"
        xcrun notarytool submit "$(PACKAGE)" --keychain-profile "$PROFILE" --wait
        xcrun stapler staple "$APP"
        pack   # again, with the ticket inside the app
        if [ $DMG = 1 ]; then
            codesign --force --timestamp -s "$IDENTITY" "$DMG_FILE"
            xcrun notarytool submit "$DMG_FILE" --keychain-profile "$PROFILE" --wait
            xcrun stapler staple "$DMG_FILE"
            spctl --assess --type open --context context:primary-signature --verbose=2 "$DMG_FILE"
        fi
        spctl --assess --type execute --verbose=2 "$APP"
    else
        echo "warning: no notarytool profile $PROFILE: signed but not notarized, Gatekeeper will still warn.
  Make it once (docs/usage.md#releases): xcrun notarytool store-credentials $PROFILE --apple-id <Apple ID> --team-id <team ID>" >&2
    fi
fi
if [ "$IDENTITY" = - ]; then
    echo "warning: no Developer ID Application certificate: this package is not signed for distribution,
  Gatekeeper will block it on other Macs (docs/usage.md#releases)." >&2
fi
shasum -a 256 "$(PACKAGE)"
du -h "$(PACKAGE)"
if [ $RELEASE = 1 ]; then   # what users' Gatekeeper checks, once more on the final files
    xcrun stapler validate "$DMG_FILE"
    spctl --assess --type open --context context:primary-signature --verbose=2 "$DMG_FILE"
    spctl --assess --type execute --verbose=2 "$APP"
    # Captured first: with pipefail, grep -q stopping early made codesign's SIGPIPE fail the check (v0.5.0).
    SIGNATURE="$(codesign --display --verbose=2 "$APP" 2>&1)"
    case "$SIGNATURE" in *'flags=0x10000(runtime)'*) ;; *) fail "--release: no hardened runtime on the app" ;; esac
    echo "release: $DMG_FILE signed ($IDENTITY), notarized, stapled, accepted by Gatekeeper"
fi
