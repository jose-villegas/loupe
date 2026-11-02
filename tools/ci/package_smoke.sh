#!/bin/sh
# Unpacks a download (cpack's .zip or .tar.gz) into an empty folder and renders
# every sample in it with -o, on a PATH that holds nothing: no compiler runtime,
# no Python, only what the archive and the system carry.
#
# usage: tools/ci/package_smoke.sh ARCHIVE OUT_DIR
set -eu

archive=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
out=$2
rm -rf "$out"
mkdir -p "$out/unpacked" "$out/renders" "$out/empty-path"
(cd "$out/unpacked" && cmake -E tar xf "$archive")

loupe=$(find "$out/unpacked" -type f \( -name loupe -o -name loupe.exe \))
[ -n "$loupe" ] || { echo "package_smoke: no loupe in $archive" >&2; exit 1; }
samples=$(find "$out/unpacked" -type f -name '*.apak' | sort)
[ -n "$samples" ] || { echo "package_smoke: no sample in $archive" >&2; exit 1; }

# On Linux, the C library is the only shared library loupe links; SDL loads
# the display's own at run time.
if [ "$(uname -s)" = Linux ] && ldd "$loupe" >"$out/ldd.txt" 2>&1; then
    if grep -vE 'linux-vdso|ld-linux|lib(c|m|dl|pthread|rt)\.so' "$out/ldd.txt"; then
        echo "package_smoke: loupe links more than the C library, above" >&2
        exit 1
    fi
fi
env PATH="$out/empty-path" "$loupe" --version
for sample in $samples; do
    image="$out/renders/$(basename "$sample" .apak).bmp"
    env PATH="$out/empty-path" "$loupe" "$sample" -o "$image"
    [ "$(head -c 2 "$image")" = BM ] || { echo "package_smoke: $image is not a BMP" >&2; exit 1; }
    echo "package_smoke: $(basename "$sample") rendered"
done
