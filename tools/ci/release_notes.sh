#!/bin/sh
# Prints the notes of release TAG in REPO (owner/name): GitHub's generated list of
# merged pull requests, each named by its number only, without author or contributor lines.
# The body is read first, so a failed request fails the script instead of printing nothing.
set -eu
tag=$1
repo=$2
body=$(gh api "repos/$repo/releases/generate-notes" -f tag_name="$tag" --jq .body)
printf '%s\n' "$body" |
    sed -e 's| by @[^ ]* in https://github.com/[^ ]*/pull/\([0-9]*\)$| (#\1)|' \
        -e '/^## New Contributors/,/^$/d'
