#include "update/update.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <format>
#include <fstream>
#include <system_error>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

#include "options.hpp"
#include "paths.hpp"
#include "process.hpp"
#include "scratch.hpp"
#include "update/sha256.hpp"

namespace fs = std::filesystem;

namespace loupe::update {

namespace {

constexpr const char SETTINGS_FILE[] = "settings.txt";
constexpr const char FOLLOW_KEY[] = "updates=";
constexpr const char LOG_FILE[] = "update.log";
constexpr const char UPDATE_DIR[] = "update";
constexpr const char API[] = "https://api.github.com/repos/" LOUPE_REPO;
constexpr const char JSON_TYPE[] = "application/vnd.github+json";
constexpr const char SHA_TYPE[] = "application/vnd.github.sha";
constexpr const char DOWNLOAD_TYPE[] = "application/octet-stream";
constexpr const char DIGEST_PREFIX[] = "sha256:";
constexpr size_t SHA256_HEX = 64;
// What a portable swap renames each file it replaces to: put back when the
// swap fails, removed when it succeeds; the running executable's at the next start.
constexpr const char ASIDE_SUFFIX[] = ".old";
// What a running executable is renamed to when a swap is undone at start,
// since it cannot be removed while it runs; removed at the next start.
constexpr const char DISCARD_SUFFIX[] = ".discard";
// The temporary folders a setup is run from, each new and named at random.
constexpr const char SETUP_FOLDER_PREFIX[] = "loupe-setup-";

// The words settings.txt spells each Follow with.
constexpr const char* FOLLOW_WORDS[] = {"off", "releases", "main"};

// <config>/update/'s files: the answers and the download on their way, the
// staged build with its record (stage_record()), and a swap's journal.
struct Files {
    fs::path dir, release, commit, part, staged, record, applied, unpacked, journal, tool_log;
};

Files
files_of(const Copy& copy) {
    const fs::path dir = copy.config / UPDATE_DIR;
    return {.dir = dir,
            .release = dir / "release.json",
            .commit = dir / "commit.txt",
            .part = dir / "download.part",
            .staged = dir / "staged",
            .record = dir / "staged.json",
            .applied = dir / "applied.txt",
            .unpacked = dir / "unpacked",
            .journal = dir / "swap.json",
            .tool_log = dir / "tool.log"};
}

// `file` parsed; discarded when it is missing or not JSON.
nlohmann::json
read_json(const fs::path& file) {
    try {
        return nlohmann::json::parse(read_text(file), nullptr, false);
    } catch (const nlohmann::json::exception&) {
        return nlohmann::json(nlohmann::json::value_t::discarded);
    }
}

// `object`'s string named `key`; empty when it has none.
std::string
text(const nlohmann::json& object, const char* key) {
    const auto it = object.is_object() ? object.find(key) : object.end();
    return it != object.end() && it->is_string() ? it->get<std::string>() : std::string();
}

Follow
own_channel(Channel channel) {
    switch (channel) {
        case Channel::Releases: return Follow::Releases;
        case Channel::Main: return Follow::Main;
        case Channel::None: break;
    }
    return Follow::Off;
}

// "1.2.3" as its numbers; none when any part is not a number.
std::optional<std::vector<long long>>
version_parts(const std::string& version) {
    std::vector<long long> parts;
    size_t start = 0;
    while (start <= version.size()) {
        const size_t dot = std::min(version.find('.', start), version.size());
        long long part = 0;
        if (!parse_count(version.substr(start, dot - start), 0, INT32_MAX, &part)) {
            return std::nullopt;
        }
        parts.push_back(part);
        start = dot + 1;
    }
    return parts;
}

// The commit, short, or the version an offer names.
std::string
describe(const Offer& offer) {
    return offer.version.empty() ? std::format("{} at {}", offer.tag, offer.commit.substr(0, 9)) : offer.version;
}

// A staged build as its record, staged.json, tells it: the offer, the channel
// it was staged on, and the commit of the copy that staged it.
struct Staged {
    Offer offer;
    std::string followed;
    std::string staged_by;
};

bool
write_stage_record(const Files& files, const Offer& offer, Follow follow, const std::string& staged_by) {
    const nlohmann::json record = {
        {"name", offer.name},    {"sha256", offer.sha256},   {"followed", FOLLOW_WORDS[static_cast<size_t>(follow)]},
        {"tag", offer.tag},      {"version", offer.version}, {"commit", offer.commit},
        {"staged_by", staged_by}};
    return write_lines(files.record, {record.dump(2)});
}

// The staged build; none when nothing is staged.
std::optional<Staged>
staged_build(const Files& files) {
    const nlohmann::json record = read_json(files.record);
    if (!record.is_object() || text(record, "name").empty() || text(record, "sha256").empty()) {
        return std::nullopt;
    }
    return Staged{.offer = {.tag = text(record, "tag"),
                            .version = text(record, "version"),
                            .commit = text(record, "commit"),
                            .name = text(record, "name"),
                            .url = "",
                            .sha256 = text(record, "sha256")},
                  .followed = text(record, "followed"),
                  .staged_by = text(record, "staged_by")};
}

void
clear_stage(const Files& files) {
    std::error_code error;
    fs::remove(files.record, error);
    fs::remove_all(files.staged, error);
}

// `offer`'s download into the staging folder for `copy`, verified; what happened.
std::string
stage(const Copy& copy, const Files& files, const Offer& offer, Follow follow, const Fetch& fetch) {
    const std::optional<Staged> staged = staged_build(files);
    if (staged && staged->offer.sha256 == offer.sha256
        && staged->followed == FOLLOW_WORDS[static_cast<size_t>(follow)]) {
        // Staged again by this copy, which may not be the one that staged it.
        write_stage_record(files, offer, follow, copy.id.commit);
        return std::format("{} is already staged", describe(offer));
    }
    std::string why;
    std::error_code error;
    if (!fetch(offer.url, DOWNLOAD_TYPE, files.part, &why)) {
        fs::remove(files.part, error);
        return std::format("cannot download {}: {}", offer.url, why);
    }
    const std::string digest = sha256_file(files.part);
    if (digest != offer.sha256) {
        fs::remove(files.part, error);
        return std::format("{} has SHA-256 {}, not GitHub's {}: deleted", offer.name, digest, offer.sha256);
    }
    clear_stage(files);
    fs::create_directories(files.staged, error);
    fs::rename(files.part, files.staged / offer.name, error);
    if (error || !write_stage_record(files, offer, follow, copy.id.commit)) {
        return std::format("cannot stage {} in {}", offer.name, utf8(files.staged));
    }
    return std::format("staged {}: {}", describe(offer), offer.name);
}

// One file a swap puts in place, and whether there was one to set aside.
struct Swapped {
    fs::path target;
    bool had_one;
};

fs::path
aside(const fs::path& file) {
    return fs::path(file) += ASIDE_SUFFIX;
}

// A swap's journal, swap.json, written before its first file and again,
// marked done, after its last: what settle() finishes or undoes after a crash.
bool
write_journal(const Files& files, const std::vector<Swapped>& swap, bool done) {
    nlohmann::json targets = nlohmann::json::array();
    for (const Swapped& swapped : swap) {
        targets.push_back({{"target", utf8(swapped.target)}, {"had_one", swapped.had_one}});
    }
    return write_lines(files.journal, {nlohmann::json{{"files", targets}, {"done", done}}.dump(2)});
}

// The swap a journal holds, and whether it was done; none when there is no journal.
std::optional<std::pair<std::vector<Swapped>, bool>>
read_journal(const Files& files) {
    std::error_code error;
    if (!fs::exists(files.journal, error)) {
        return std::nullopt;
    }
    const nlohmann::json journal = read_json(files.journal);
    const auto targets = journal.is_object() ? journal.find("files") : journal.end();
    std::vector<Swapped> swap;
    for (const nlohmann::json& file : targets != journal.end() && targets->is_array() ? *targets : nlohmann::json()) {
        const auto had_one = file.is_object() ? file.find("had_one") : file.end();
        if (!text(file, "target").empty() && had_one != file.end() && had_one->is_boolean()) {
            swap.push_back({.target = from_utf8(text(file, "target")), .had_one = had_one->get<bool>()});
        }
    }
    const auto done = journal.is_object() ? journal.find("done") : journal.end();
    return std::pair{swap, done != journal.end() && done->is_boolean() && done->get<bool>()};
}

// `source` in place of `swapped`'s target, which is renamed aside first; false with `why`.
bool
put(const fs::path& source, const Swapped& swapped, std::string* why) {
    std::error_code error;
    fs::create_directories(swapped.target.parent_path(), error);
    if (swapped.had_one) {
        fs::remove(aside(swapped.target), error);
        fs::rename(swapped.target, aside(swapped.target), error);
        if (error) {
            *why = std::format("cannot rename {} aside: {}", utf8(swapped.target), error.message());
            return false;
        }
    }
    if (!fs::copy_file(source, swapped.target, error)) {
        *why = std::format("cannot write {}: {}", utf8(swapped.target), error.message());
        return false;
    }
    fs::permissions(swapped.target, fs::status(source, error).permissions(), error);
    return true;
}

// Every target as it was before the swap, last first: a new file removed (a
// running one renamed out of the way) and the one set aside put back. A
// target whose old file was never set aside was not reached.
void
undo(const std::vector<Swapped>& swap) {
    std::error_code error;
    for (auto it = swap.rbegin(); it != swap.rend(); ++it) {
        if (it->had_one && !fs::exists(aside(it->target), error)) {
            continue;
        }
        if (!fs::remove(it->target, error) && fs::exists(it->target, error)) {
            fs::rename(it->target, fs::path(it->target) += DISCARD_SUFFIX, error);
        }
        if (it->had_one) {
            fs::rename(aside(it->target), it->target, error);
        }
    }
}

// The files set aside removed, but for the running executable `running`,
// which settle() removes at the next start.
void
finish(const std::vector<Swapped>& swap, const fs::path& running) {
    std::error_code error;
    for (const Swapped& swapped : swap) {
        if (swapped.target != running) {
            fs::remove(aside(swapped.target), error);
        }
    }
}

// The file in `dir`'s tree named as `copy`'s executable; empty when there is none.
fs::path
find_exe(const fs::path& dir, const Copy& copy) {
    const fs::path name = copy.exe.filename();
    std::error_code error;
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(dir, error)) {
        if (entry.is_regular_file(error) && entry.path().filename() == name) {
            return entry.path();
        }
    }
    return {};
}

// The archive unpacked in the update folder, then each of the copy's files,
// the running executable among them, renamed aside and replaced; when one
// cannot be, every one is put back, so the copy is whole either way. A crash
// part way leaves the journal, and settle() puts them back at the next start.
bool
swap_portable(const Copy& copy, const Files& files, const fs::path& archive, std::string* why) {
    std::error_code error;
    fs::remove_all(files.unpacked, error);
    fs::create_directories(files.unpacked, error);
    const fs::path tar = system_program("tar");
    if (tar.empty() || !run_logged({utf8(tar), "-xf", utf8(archive), "-C", utf8(files.unpacked)}, files.tool_log)) {
        *why = std::format("cannot unpack {} (see {})", utf8(archive), utf8(files.tool_log));
        return false;
    }
    const fs::path exe = find_exe(files.unpacked, copy);
    if (exe.empty()) {
        *why = std::format("{} holds no {}", utf8(archive.filename()), utf8(copy.exe.filename()));
        return false;
    }
    // The archive's root folder (CPack's archives hold one), and the copy's
    // folder that stands for it: as many levels above the executable as the
    // archive's is below its root.
    const fs::path top = files.unpacked / *fs::relative(exe, files.unpacked).begin();
    fs::path root = copy.exe.parent_path();
    for (const fs::path& level : fs::relative(exe, top).parent_path()) {
        (void)level;
        root = root.parent_path();
    }
    std::vector<fs::path> sources;
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(top, error)) {
        if (entry.is_regular_file(error)) {
            sources.push_back(entry.path());
        }
    }
    std::ranges::sort(sources);
    std::vector<Swapped> swap;
    swap.reserve(sources.size());
    for (const fs::path& source : sources) {
        const fs::path target = root / fs::relative(source, top);
        swap.push_back({.target = target, .had_one = fs::exists(target, error)});
    }
    bool swapped = write_journal(files, swap, false);
    if (!swapped) {
        *why = std::format("cannot write {}", utf8(files.journal));
    }
    for (size_t i = 0; swapped && i < swap.size(); i++) {
        swapped = put(sources[i], swap[i], why);
    }
    if (swapped && !write_journal(files, swap, true)) {
        swapped = false;
        *why = std::format("cannot write {}", utf8(files.journal));
    }
    if (swapped) {
        finish(swap, copy.exe);
    } else {
        undo(swap);
    }
    fs::remove(files.journal, error);
    fs::remove_all(files.unpacked, error);
    return swapped;
}

// The staged setup copied into a fresh folder, so nothing beside it there
// (a DLL it would load) was put by anyone else, then run as hashed: no one
// can change it between the two.
bool
run_setup(const fs::path& file, const std::string& sha256) {
    // Kept: the setup runs on after Loupe has closed; settle() removes it.
    Scratch folder(SETUP_FOLDER_PREFIX);
    folder.keep();
    const fs::path setup = folder.path() / file.filename();
    std::error_code error;
    return !folder.path().empty() && fs::copy_file(file, setup, error)
           && run_verified(setup, sha256, [&setup] { return start_unwaited(setup, {"/S"}); });
}

} // namespace

Follow
follow(const Copy& copy) {
    const Follow own = own_channel(copy.id.built_for);
    if (own == Follow::Off || copy.config.empty()) {
        return Follow::Off;
    }
    for (const std::string& line : read_lines(copy.config / SETTINGS_FILE)) {
        if (!line.starts_with(FOLLOW_KEY)) {
            continue;
        }
        for (size_t i = 0; i < std::size(FOLLOW_WORDS); i++) {
            if (line.substr(std::string_view(FOLLOW_KEY).size()) == FOLLOW_WORDS[i]) {
                return static_cast<Follow>(i);
            }
        }
    }
    return own;
}

bool
set_follow(const Copy& copy, Follow follow) {
    if (copy.config.empty()) {
        return false;
    }
    const fs::path file = copy.config / SETTINGS_FILE;
    std::vector<std::string> lines;
    for (std::string& line : read_lines(file)) {
        if (!line.starts_with(FOLLOW_KEY)) {
            lines.push_back(std::move(line));
        }
    }
    lines.push_back(std::string(FOLLOW_KEY) + FOLLOW_WORDS[static_cast<size_t>(follow)]);
    return write_lines(file, lines);
}

std::string_view
asset_suffix(const Copy& copy) {
#ifdef LOUPE_SETUP_SUFFIX
    if (copy.id.installed) {
        return LOUPE_SETUP_SUFFIX;
    }
#else
    (void)copy;
#endif
    return LOUPE_PORTABLE_SUFFIX;
}

std::optional<Offer>
parse_offer(const nlohmann::json& release, std::string_view suffix, std::string* why) {
    Offer offer;
    offer.tag = text(release, "tag_name");
    const auto assets = release.is_object() ? release.find("assets") : release.end();
    if (offer.tag.empty() || assets == release.end() || !assets->is_array()) {
        *why = "the release has no tag or no assets";
        return std::nullopt;
    }
    if (offer.tag.starts_with('v')) {
        offer.version = offer.tag.substr(1, offer.tag.find('-') - 1);
    }
    for (const nlohmann::json& asset : *assets) {
        const std::string name = text(asset, "name");
        if (!name.ends_with(suffix)) {
            continue;
        }
        const std::string digest = text(asset, "digest");
        const std::string url = text(asset, "browser_download_url");
        if (!digest.starts_with(DIGEST_PREFIX) || digest.size() != std::string_view(DIGEST_PREFIX).size() + SHA256_HEX
            || url.empty()) {
            *why = std::format("{} has no SHA-256 digest or no download URL", name);
            return std::nullopt;
        }
        offer.name = name;
        offer.url = url;
        offer.sha256 = digest.substr(std::string_view(DIGEST_PREFIX).size());
        return offer;
    }
    *why = std::format("{} has no download named *{}", offer.tag, suffix);
    return std::nullopt;
}

bool
is_newer(const Offer& offer, const BuildId& id, Follow follow) {
    if (follow == Follow::Main) {
        return !offer.commit.empty() && id.commit != "unknown" && !offer.commit.starts_with(id.commit);
    }
    if (follow != Follow::Releases) {
        return false;
    }
    const std::optional<std::vector<long long>> offered = version_parts(offer.version);
    const std::optional<std::vector<long long>> own = version_parts(id.version);
    return offered && own && *offered > *own;
}

Fetch
curl(const fs::path& log, const std::stop_token& stop) {
    return [log, stop](const std::string& url, const std::string& accept, const fs::path& out, std::string* why) {
        const fs::path program = system_program("curl");
        if (program.empty()) {
            *why = "no curl on this system";
            return false;
        }
        std::error_code error;
        fs::create_directories(out.parent_path(), error);
        if (run_logged({utf8(program), "-fsSL", "--retry", "2", "--max-time", "600", "-H", "Accept: " + accept, "-o",
                        utf8(out), url},
                       log, stop)) {
            return true;
        }
        if (stop.stop_requested()) {
            *why = "stopped: Loupe closed";
            return false;
        }
        const std::vector<std::string> said = read_lines(log);
        *why = said.empty() ? "curl failed" : said.back();
        return false;
    };
}

std::string
check(const Copy& copy, Follow follow, const Fetch& fetch) {
    if (copy.id.built_for == Channel::None) {
        return "a local build never updates";
    }
    if (follow == Follow::Off || copy.config.empty()) {
        return "updates are off";
    }
    const Files files = files_of(copy);
    const std::string tag = follow == Follow::Releases ? std::string() : LOUPE_MAIN_TAG;
    const std::string release_url = std::format("{}/releases/{}", API, tag.empty() ? "latest" : "tags/" + tag);
    std::string why;
    if (!fetch(release_url, JSON_TYPE, files.release, &why)) {
        return std::format("cannot ask {}: {}", release_url, why);
    }
    const nlohmann::json release = read_json(files.release);
    if (release.is_discarded()) {
        return std::format("{} answered no JSON", release_url);
    }
    std::optional<Offer> offer = parse_offer(release, asset_suffix(copy), &why);
    if (!offer) {
        return why;
    }
    if (follow == Follow::Main) {
        const std::string commit_url = std::format("{}/commits/{}", API, offer->tag);
        if (!fetch(commit_url, SHA_TYPE, files.commit, &why)) {
            return std::format("cannot ask {}: {}", commit_url, why);
        }
        const std::vector<std::string> commit = read_lines(files.commit);
        offer->commit = commit.empty() ? "" : commit[0];
    }
    if (!is_newer(*offer, copy.id, follow)) {
        clear_stage(files);
        return std::format("up to date: {} offers {}", release_url, describe(*offer));
    }
    return stage(copy, files, *offer, follow, fetch);
}

std::jthread
check_in_background(const Copy& copy, Follow follow) {
    if (copy.id.built_for == Channel::None || follow == Follow::Off || copy.config.empty()) {
        return {};
    }
    return std::jthread([copy, follow](const std::stop_token& stop) {
        log(copy, check(copy, follow, curl(files_of(copy).tool_log, stop)));
    });
}

bool
apply_staged(const Copy& copy, Follow follow, std::string* why) {
    why->clear();
    if (copy.config.empty()) {
        return false;
    }
    const Files files = files_of(copy);
    const std::optional<Staged> staged = staged_build(files);
    if (!staged) {
        return false;
    }
    const Offer& offer = staged->offer;
    // Staged by another build: this one may be newer than it, put in by hand.
    if (staged->staged_by != copy.id.commit) {
        *why = std::format("the staged {} was staged by {}, not this copy's {}: cleared", describe(offer),
                           staged->staged_by, copy.id.commit);
        clear_stage(files);
        return false;
    }
    if (staged->followed != FOLLOW_WORDS[static_cast<size_t>(follow)] || !is_newer(offer, copy.id, follow)) {
        *why = std::format("the staged {} is not newer on what this copy follows now: cleared", describe(offer));
        clear_stage(files);
        return false;
    }
    const fs::path file = files.staged / offer.name;
    // Read at the next start: a copy of another commit then is the new build.
    if (!write_lines(files.applied, {copy.id.commit})) {
        *why = std::format("cannot write {}", utf8(files.applied));
        return false;
    }
    const bool applied = copy.id.installed ? run_setup(file, offer.sha256)
                                           : sha256_file(file) == offer.sha256 && swap_portable(copy, files, file, why);
    std::error_code error;
    if (!applied) {
        fs::remove(files.applied, error);
        if (why->empty()) {
            *why = std::format("{} changed since it was staged, or would not start: cleared", utf8(file));
            clear_stage(files);
        }
        return false;
    }
    fs::remove(files.record, error);
    if (!copy.id.installed) {
        fs::remove_all(files.staged, error);
    }
    return true;
}

std::optional<std::string>
settle(const Copy& copy) {
    std::error_code error;
    for (const fs::directory_entry& entry : fs::directory_iterator(fs::temp_directory_path(error), error)) {
        if (entry.path().filename().string().starts_with(SETUP_FOLDER_PREFIX)) {
            fs::remove_all(entry.path(), error);
        }
    }
    if (copy.config.empty()) {
        return std::nullopt;
    }
    const Files files = files_of(copy);
    // A swap cut short: done, its files set aside are removed; else every file is put back.
    const std::optional<std::pair<std::vector<Swapped>, bool>> journal = read_journal(files);
    if (journal && journal->second) {
        finish(journal->first, {});
    } else if (journal) {
        undo(journal->first);
        fs::remove(files.applied, error);
        log(copy, "a swap that did not finish was undone");
    }
    fs::remove(files.journal, error);
    if (!copy.exe.empty()) {
        fs::remove(aside(copy.exe), error);
        fs::remove(fs::path(copy.exe) += DISCARD_SUFFIX, error);
    }
    const std::vector<std::string> applied = read_lines(files.applied);
    if (applied.empty()) {
        return std::nullopt;
    }
    fs::remove(files.applied, error);
    fs::remove_all(files.staged, error);
    if (applied[0] == copy.id.commit) {
        log(copy, "the staged build did not take this copy's place");
        return std::nullopt;
    }
    std::string updated = std::format("Updated to {} ({})", copy.id.version, copy.id.commit);
    log(copy, updated);
    return updated;
}

void
log(const Copy& copy, const std::string& line) {
    if (copy.config.empty()) {
        return;
    }
    std::error_code error;
    fs::create_directories(copy.config, error);
    std::ofstream out(copy.config / LOG_FILE, std::ios::binary | std::ios::app);
    out << std::format("{:%FT%TZ} ", std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now())) << line
        << '\n';
}

} // namespace loupe::update
