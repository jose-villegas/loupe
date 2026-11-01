#!/bin/sh
# Installs tools/ci/linux-packages.txt with apt (Debian/Ubuntu).
set -eu
packages=$(grep -v '^#' "$(dirname "$0")/linux-packages.txt")
sudo apt-get update
# shellcheck disable=SC2086 # one word per package
sudo apt-get install -y --no-install-recommends $packages
