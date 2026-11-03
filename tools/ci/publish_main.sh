#!/bin/sh
# Points the rolling pre-release TAG of REPO (owner/name) at COMMIT and gives it
# DIST's files, keeping their names: those of a Main build, loupe-main-*. The
# release is edited in place, not made anew, so its links and its watchers'
# notifications stay as they are; files gone from DIST leave it too. Its notes
# name the commit and list the pull requests merged since the latest release.
# A COMMIT that main has moved past publishes nothing: main's own run does.
#
# usage: tools/ci/publish_main.sh TAG REPO COMMIT DIST
set -eu
tag=$1
repo=$2
commit=$3
dist=$4

ls "$dist"/loupe-main-* >/dev/null 2>&1 || { echo "publish_main: no Main build in $dist: $(ls "$dist")" >&2; exit 1; }
head=$(gh api "repos/$repo/commits/main" --jq .sha)
if [ "$head" != "$commit" ]; then
    echo "publish_main: main is at $head now, past $commit; its own run publishes it"
    exit 0
fi

if gh api "repos/$repo/git/refs/tags/$tag" >/dev/null 2>&1; then
    gh api -X PATCH "repos/$repo/git/refs/tags/$tag" -f sha="$commit" -F force=true >/dev/null
else
    gh api "repos/$repo/git/refs" -f ref="refs/tags/$tag" -f sha="$commit" >/dev/null
fi

latest=$(gh release view --repo "$repo" --json tagName --jq .tagName)
{
    echo "The build of main at $commit, replaced on every push to main."
    echo
    sh "$(dirname "$0")/release_notes.sh" "$tag" "$repo" "$latest"
} >"$dist.notes.md"

if gh release view "$tag" --repo "$repo" >/dev/null 2>&1; then
    gh release edit "$tag" --repo "$repo" --prerelease --notes-file "$dist.notes.md"
    gh release upload "$tag" "$dist"/* --repo "$repo" --clobber
    gh release view "$tag" --repo "$repo" --json assets --jq '.assets[].name' | while read -r asset; do
        [ -f "$dist/$asset" ] || gh release delete-asset "$tag" "$asset" --repo "$repo" --yes
    done
else
    gh release create "$tag" "$dist"/* --repo "$repo" --verify-tag --prerelease \
        --title "Loupe main" --notes-file "$dist.notes.md"
fi
echo "publish_main: $tag is $commit"
