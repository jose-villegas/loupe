#!/bin/sh
# Prints the notes of release TAG in REPO (owner/name): GitHub's generated list of
# merged pull requests, each named by its number only, without author or contributor lines.
set -eu
tag=$1
repo=$2
gh api "repos/$repo/releases/generate-notes" -f tag_name="$tag" --jq .body |
    sed -e 's| by @[^ ]* in https://github.com/[^ ]*/pull/\([0-9]*\)$| (#\1)|' \
        -e '/^## New Contributors/,/^$/d'
