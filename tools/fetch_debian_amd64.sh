#!/bin/bash
# Linux x86-64 system libraries for P2CE (round 28): libstdc++.so.6 and
# libgcc_s.so.1, which P2CE's libvideo.so links dynamically (its other modules
# carry their C++ runtime in libmimalloc.so). Extracted from Debian bullseye's
# gcc-10 amd64 packages, SHA-256 checked against Debian's package index, into
# deps/debian-amd64/lib, the way setup.sh fetches the i386 ones for Mel and L4D2.
# Nothing is committed: they're downloaded on the user's machine.
#   tools/fetch_debian_amd64.sh        prints the folder holding them
set -euo pipefail
REPO="$(cd "$(dirname "$0")/.." && pwd)"
DEST="${P2_DEPS:-$REPO/deps}/debian-amd64"
URL="https://deb.debian.org/debian/pool/main/g/gcc-10"
PACKAGES=(
    "libstdc++6_10.2.1-6_amd64.deb:5c155c58935870bf3b4bfe769116841c0d286a74f59eccfd5645693ac23f06b1:libstdc++.so.6"
    "libgcc-s1_10.2.1-6_amd64.deb:e478f2709d8474165bb664de42e16950c391f30eaa55bc9b3573281d83a29daf:libgcc_s.so.1"
)
mkdir -p "$DEST/lib"
for spec in "${PACKAGES[@]}"; do
    deb="${spec%%:*}"; rest="${spec#*:}"; sha="${rest%%:*}"; lib="${rest#*:}"
    [ -f "$DEST/lib/$lib" ] && continue
    if [ ! -f "$DEST/$deb" ]; then
        curl -fsSL --retry 3 -o "$DEST/$deb.part" "$URL/$deb" && mv "$DEST/$deb.part" "$DEST/$deb" \
            || { rm -f "$DEST/$deb.part"; echo "could not download $URL/$deb" >&2; exit 1; }
    fi
    got=$(shasum -a 256 "$DEST/$deb" | cut -d' ' -f1)
    [ "$got" = "$sha" ] || { echo "$DEST/$deb: SHA-256 $got, expected $sha (delete it and re-run)" >&2; exit 1; }
    t=$(mktemp -d)
    (cd "$t" && ar x "$DEST/$deb" && tar -xf data.tar.*)
    found=$(find "$t" -name "$lib" \( -type f -o -type l \) | head -1)
    [ -n "$found" ] || { rm -rf "$t"; echo "$lib not found in $deb" >&2; exit 1; }
    cp -L "$found" "$DEST/lib/$lib"
    rm -rf "$t"
done
echo "$DEST/lib"
