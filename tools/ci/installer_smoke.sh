#!/bin/sh
# Installs the Windows setup download silently into an empty folder, checks
# its shortcuts and its .apak association, renders every sample from the
# installed loupe.exe (package_smoke.sh), then uninstalls silently and checks
# that nothing is left: no folder, shortcut or registry key. Needs an admin
# shell, as the installer writes for all users.
#
# usage: tools/ci/installer_smoke.sh SETUP_EXE OUT_DIR
set -eu
# /S, /D=, /ve and /reg:64 are the programs' switches, not paths to convert.
export MSYS2_ARG_CONV_EXCL='*'

setup=$(cygpath -w "$1")
out=$2
dir=$out/installed
start_menu="$(cygpath -u "$PROGRAMDATA")/Microsoft/Windows/Start Menu/Programs/Loupe"
desktop_link="$(cygpath -u "$PUBLIC")/Desktop/Loupe.lnk"
# The installer is a 32-bit program: HKLM\Software\Classes is shared, the rest
# lands in the 32-bit view.
keys='HKLM\Software\Classes\.apak
HKLM\Software\Classes\Loupe.apak
HKLM\Software\Microsoft\Windows\CurrentVersion\Uninstall\Loupe
HKLM\Software\jose-villegas\Loupe'

fail() {
    echo "installer_smoke: $*" >&2
    exit 1
}

rm -rf "$out"
mkdir -p "$out"
# /D must be last and unquoted.
"$setup" /S "/D=$(cygpath -w "$dir")" || fail "the installer exited $?"

[ -f "$dir/loupe.exe" ] || fail "no loupe.exe in $dir"
[ -f "$dir/Uninstall.exe" ] || fail "no Uninstall.exe in $dir"
[ -f "$start_menu/Loupe.lnk" ] || fail "no Start menu entry in $start_menu"
[ -f "$desktop_link" ] || fail "no desktop shortcut at $desktop_link"
reg query 'HKLM\Software\Classes\.apak' /ve | grep -q 'Loupe\.apak' || fail ".apak does not name Loupe.apak"
command=$(reg query 'HKLM\Software\Classes\Loupe.apak\shell\open\command' /ve)
echo "$command" | grep -qF "\"$(cygpath -w "$dir")\loupe.exe\" \"%1\"" || fail "Loupe.apak does not open the installed loupe.exe: $command"
echo "installer_smoke: installed, with shortcuts and the .apak association"

tools/ci/package_smoke.sh "$dir" "$out/smoke"

# The uninstaller copies itself to %TEMP% and returns at once; the copy
# removes the folder, so wait up to a minute for that.
"$(cygpath -w "$dir")\Uninstall.exe" /S || fail "the uninstaller exited $?"
# shellcheck disable=SC2016 # $1 is the inner shell's.
timeout 60 sh -c 'while [ -e "$1" ]; do sleep 1; done' _ "$dir" || true

left=
[ ! -e "$dir" ] || left="$left $dir:$(find "$dir" | tr '\n' ' ')"
[ ! -e "$start_menu" ] || left="$left $start_menu"
[ ! -e "$desktop_link" ] || left="$left $desktop_link"
for key in $keys; do
    for view in 32 64; do
        ! reg query "$key" "/reg:$view" >/dev/null 2>&1 || left="$left $key(/reg:$view)"
    done
done
[ -z "$left" ] || fail "left behind after uninstall:$left"
echo "installer_smoke: uninstalled, nothing left"
