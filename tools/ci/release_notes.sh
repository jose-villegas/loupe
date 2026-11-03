#!/bin/sh
# Prints the notes of release TAG in REPO (owner/name): GitHub's generated list of
# merged pull requests, since the release PREVIOUS when given, each named by its
# number only, without author or contributor lines.
# The body is read first, so a failed request fails the script instead of printing nothing.
set -eu
tag=$1
repo=$2
previous=${3:-}
body=$(gh api "repos/$repo/releases/generate-notes" -f tag_name="$tag" ${previous:+-f previous_tag_name="$previous"} --jq .body)
printf '%s\n' "$body" |
    sed -e 's| by @[^ ]* in https://github.com/[^ ]*/pull/\([0-9]*\)$| (#\1)|' \
        -e '/^## New Contributors/,/^$/d'
