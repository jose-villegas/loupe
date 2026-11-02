#!/bin/sh
# Installs the Windows setup download silently into an empty folder, twice,
# so the second replaces the first as an upgrade does. Checks its shortcuts
# and its .apak association, renders every sample from the installed
# loupe.exe (package_smoke.sh), then uninstalls silently and checks that
# .apak opens what it opened before and nothing else is left: no folder,
# shortcut or registry key. Needs an admin shell, as the installer writes for
# all users.
#
# usage: tools/ci/installer_smoke.sh SETUP_EXE OUT_DIR
set -eu

setup=$(cygpath -w "$1")
out=$2
dir=$out/installed
start_menu="$(cygpath -u "$ALLUSERSPROFILE")/Microsoft/Windows/Start Menu/Programs/Loupe"
desktop_link="$(cygpath -u "$PUBLIC")/Desktop/Loupe.lnk"
apak='HKLM\Software\Classes\.apak'
# The installer is a 32-bit program: HKLM\Software\Classes is shared, the rest
# lands in the 32-bit view.
keys="$apak
HKLM\Software\Classes\Loupe.apak
HKLM\Software\Microsoft\Windows\CurrentVersion\Uninstall\Loupe
HKLM\Software\jose-villegas"

fail() {
    echo "installer_smoke: $*" >&2
    exit 1
}
# Windows programs' switches (/S, /D=, /ve, /reg:64), not paths for Git Bash to convert.
windows() {
    MSYS2_ARG_CONV_EXCL='*' "$@"
}
default_of() {
    windows reg query "$1" /ve | sed -n 's/.*REG_SZ *//p' | tr -d '\r'
}

rm -rf "$out"
mkdir -p "$out"
# A program that opened .apak before Loupe, to be given it back.
windows reg add "$apak" /ve /d Other.apak /f >/dev/null
for pass in 1 2; do
    # /D must be last and unquoted.
    windows "$setup" /S "/D=$(cygpath -w "$dir")" || fail "install $pass exited $?"
done

[ -f "$dir/loupe.exe" ] || fail "no loupe.exe in $dir"
[ -f "$dir/Uninstall.exe" ] || fail "no Uninstall.exe in $dir"
[ -f "$start_menu/Loupe.lnk" ] || fail "no Start menu entry in $start_menu"
[ -f "$desktop_link" ] || fail "no desktop shortcut at $desktop_link"
[ "$(default_of "$apak")" = Loupe.apak ] || fail ".apak does not open Loupe.apak"
command=$(default_of 'HKLM\Software\Classes\Loupe.apak\shell\open\command')
[ "$command" = "\"$(cygpath -w "$dir")\loupe.exe\" \"%1\"" ] || fail "Loupe.apak opens $command, not the installed loupe.exe"
echo "installer_smoke: installed twice, with shortcuts and the .apak association"

tools/ci/package_smoke.sh "$dir" "$out/smoke"

# The uninstaller copies itself to %TEMP% and returns at once; the copy
# removes the folder, so wait up to a minute for that.
windows "$(cygpath -w "$dir")\Uninstall.exe" /S || fail "the uninstaller exited $?"
# shellcheck disable=SC2016 # $1 is the inner shell's.
timeout 60 sh -c 'while [ -e "$1" ]; do sleep 1; done' _ "$dir" || true

previous=$(default_of "$apak")
[ "$previous" = Other.apak ] || fail ".apak opens '$previous' after uninstall, not Other.apak again"
windows reg delete "$apak" /f >/dev/null
left=
[ ! -e "$dir" ] || left="$left $dir:$(find "$dir" | tr '\n' ' ')"
[ ! -e "$start_menu" ] || left="$left $start_menu"
[ ! -e "$desktop_link" ] || left="$left $desktop_link"
while IFS= read -r key; do
    for view in 32 64; do
        ! windows reg query "$key" "/reg:$view" >/dev/null 2>&1 || left="$left $key(/reg:$view)"
    done
done <<KEYS
$keys
KEYS
[ -z "$left" ] || fail "left behind after uninstall:$left"
echo "installer_smoke: uninstalled, .apak given back, nothing left"
