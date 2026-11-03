// Loupe keeps itself current from its GitHub releases. Once per window start
// it asks, on its own thread, what the followed channel offers; a newer build
// is downloaded, checked against GitHub's SHA-256 digest and staged in
// <config>/update/. When the window closes, the staged build takes this copy's
// place, so the next start is the new one. A local build (Channel::None)
// never asks. Every step is logged to <config>/update.log; nothing pops up.
#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

#include "update/json.hpp"
#include "version.hpp"

namespace loupe::update {

// What a copy follows: nothing, the official releases, or the build of main.
enum class Follow : uint8_t { Off, Releases, Main };

// A copy of loupe: its id, its running executable, and loupe's settings folder.
struct Copy {
    BuildId id;
    std::filesystem::path exe;
    std::filesystem::path config;
};

// <config>/settings.txt's `updates=` line; when unset, the channel the copy
// was built for. Off for a local build, whatever the file says.
Follow follow(const Copy& copy);
// Writes `follow` to <config>/settings.txt, keeping its other lines; false
// when it cannot be written.
bool set_follow(const Copy& copy, Follow follow);

// What a channel offers this copy: its release's tag, its version (from a
// v<version> tag; empty for the main build), its commit (asked for the main
// build only), and the one download for this copy with its SHA-256.
struct Offer {
    std::string tag, version, commit, name, url, sha256;
};

// The end of this copy's download's name: the setup for an installed copy,
// the portable archive otherwise.
std::string_view asset_suffix(const Copy& copy);
// `release`, GitHub's JSON for one release, as an offer of its asset named
// with `suffix`; none, with `why`, when it has no such asset or no digest.
std::optional<Offer> parse_offer(const Json& release, std::string_view suffix, std::string* why);
// Whether `offer` replaces `id` on `follow`'s channel: a release of a higher
// version, or a main build of another commit. Never on Off.
bool is_newer(const Offer& offer, const BuildId& id, Follow follow);

// Fetches `url`, asking for `accept`, into `out`; false with `why` when it failed.
using Fetch = std::function<bool(const std::string& url, const std::string& accept, const std::filesystem::path& out,
                                 std::string* why)>;
// A Fetch through the system's curl, its output into `log`.
Fetch curl(const std::filesystem::path& log);

// One check of `follow`'s channel through `fetch`; stages a newer, verified
// build. What it did, as one line for update.log.
std::string check(const Copy& copy, Follow follow, const Fetch& fetch);
// check() through curl on its own thread, logged; nothing for a local build,
// Off, or no settings folder.
void check_in_background(const Copy& copy, Follow follow);

// Puts the staged build in place of `copy`: a portable copy's files are
// replaced from the archive, its running executable renamed aside first; an
// installed copy's setup runs silently, asking Windows for elevation. False
// with `why` empty when nothing is staged, or set when the swap failed.
bool apply_staged(const Copy& copy, std::string* why);
// At start: removes what the last swap set aside; "Updated to <version> (<commit>)"
// when `copy` is a build swapped in since the last start.
std::optional<std::string> settle(const Copy& copy);
// `line`, timestamped, appended to <config>/update.log.
void log(const Copy& copy, const std::string& line);

} // namespace loupe::update
