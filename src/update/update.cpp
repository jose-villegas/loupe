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

#include "options.hpp"
#include "paths.hpp"
#include "process.hpp"
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

// The words settings.txt spells each Follow with.
constexpr const char* FOLLOW_WORDS[] = {"off", "releases", "main"};

// <config>/update/'s files: the answers and the download on their way, and
// the staged build with its record (stage_record()).
struct Files {
    fs::path dir, release, commit, part, staged, record, applied, unpacked, tool_log;
};

Files
files_of(const Copy& copy) {
    const fs::path dir = copy.config / UPDATE_DIR;
    return {.dir = dir,
            .release = dir / "release.json",
            .commit = dir / "commit.txt",
            .part = dir / "download.part",
            .staged = dir / "staged",
            .record = dir / "staged.txt",
            .applied = dir / "applied.txt",
            .unpacked = dir / "unpacked",
            .tool_log = dir / "tool.log"};
}

std::vector<std::string>
lines_of(const fs::path& file) {
    std::vector<std::string> lines;
    std::ifstream in(file, std::ios::binary);
    std::string line;
    while (std::getline(in, line)) {
        if (line.ends_with('\r')) {
            line.pop_back();
        }
        lines.push_back(line);
    }
    return lines;
}

bool
write_lines(const fs::path& file, const std::vector<std::string>& lines) {
    std::error_code error;
    fs::create_directories(file.parent_path(), error);
    const fs::path temp = fs::path(file) += ".tmp";
    {
        std::ofstream out(temp, std::ios::binary | std::ios::trunc);
        for (const std::string& line : lines) {
            out << line << '\n';
        }
        if (!out.good()) {
            return false;
        }
    }
    fs::rename(temp, file, error);
    return !error;
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

// A staged build's record, one line each: its file's name, its SHA-256, the
// channel it was staged on, its tag, version and commit.
enum RecordLine : uint8_t { NAME, SHA256, FOLLOWED, TAG, VERSION, COMMIT, RECORD_LINES };

std::vector<std::string>
stage_record(const Offer& offer, Follow follow) {
    std::vector<std::string> record(RECORD_LINES);
    record[NAME] = offer.name;
    record[SHA256] = offer.sha256;
    record[FOLLOWED] = FOLLOW_WORDS[static_cast<size_t>(follow)];
    record[TAG] = offer.tag;
    record[VERSION] = offer.version;
    record[COMMIT] = offer.commit;
    return record;
}

// The staged build's offer and the channel it was staged on; none when nothing is staged.
std::optional<std::pair<Offer, std::string>>
staged_offer(const Files& files) {
    const std::vector<std::string> record = lines_of(files.record);
    if (record.size() != RECORD_LINES) {
        return std::nullopt;
    }
    return std::pair{Offer{.tag = record[TAG],
                           .version = record[VERSION],
                           .commit = record[COMMIT],
                           .name = record[NAME],
                           .url = "",
                           .sha256 = record[SHA256]},
                     record[FOLLOWED]};
}

void
clear_stage(const Files& files) {
    std::error_code error;
    fs::remove(files.record, error);
    fs::remove_all(files.staged, error);
}

// `offer`'s download into the staging folder, verified; what happened.
std::string
stage(const Files& files, const Offer& offer, Follow follow, const Fetch& fetch) {
    const std::optional<std::pair<Offer, std::string>> staged = staged_offer(files);
    if (staged && staged->first.sha256 == offer.sha256 && staged->second == FOLLOW_WORDS[static_cast<size_t>(follow)]) {
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
    if (error || !write_lines(files.record, stage_record(offer, follow))) {
        return std::format("cannot stage {} in {}", offer.name, utf8(files.staged));
    }
    return std::format("staged {}: {}", describe(offer), offer.name);
}

// Files a swap has replaced so far, to put back when a later one fails.
class Replaced {
  public:
    Replaced() = default;
    Replaced(const Replaced&) = delete;
    Replaced& operator=(const Replaced&) = delete;
    Replaced(Replaced&&) = delete;
    Replaced& operator=(Replaced&&) = delete;

    // Unless kept: every new file removed, every replaced one renamed back.
    ~Replaced() {
        std::error_code error;
        for (auto it = done_.rbegin(); !kept_ && it != done_.rend(); ++it) {
            fs::remove(it->target, error);
            if (it->had_one) {
                fs::rename(aside(it->target), it->target, error);
            }
        }
    }

    // `source` in place of `target`, which is renamed aside first; false with `why`.
    bool
    put(const fs::path& source, const fs::path& target, std::string* why) {
        std::error_code error;
        const bool had_one = fs::exists(target, error);
        fs::create_directories(target.parent_path(), error);
        if (had_one) {
            fs::remove(aside(target), error);
            fs::rename(target, aside(target), error);
            if (error) {
                *why = std::format("cannot rename {} aside: {}", utf8(target), error.message());
                return false;
            }
        }
        done_.push_back({.target = target, .had_one = had_one});
        if (!fs::copy_file(source, target, error)) {
            *why = std::format("cannot write {}: {}", utf8(target), error.message());
            return false;
        }
        fs::permissions(target, fs::status(source, error).permissions(), error);
        return true;
    }

    // Keeps the new files; the replaced ones are removed, but for a running
    // executable, which settle() removes at the next start.
    void
    keep() {
        kept_ = true;
        std::error_code error;
        for (const Done& done : done_) {
            fs::remove(aside(done.target), error);
        }
    }

  private:
    static fs::path
    aside(const fs::path& file) {
        return fs::path(file) += ASIDE_SUFFIX;
    }

    struct Done {
        fs::path target;
        bool had_one;
    };

    std::vector<Done> done_;
    bool kept_ = false;
};

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
// cannot be, every one is put back, so the copy is whole either way.
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
    bool swapped = true;
    {
        Replaced replaced;
        for (const fs::path& source : sources) {
            if (!replaced.put(source, root / fs::relative(source, top), why)) {
                swapped = false;
                break;
            }
        }
        if (swapped) {
            replaced.keep();
        }
    }
    fs::remove_all(files.unpacked, error);
    return swapped;
}

} // namespace

Follow
follow(const Copy& copy) {
    const Follow own = own_channel(copy.id.built_for);
    if (own == Follow::Off || copy.config.empty()) {
        return Follow::Off;
    }
    for (const std::string& line : lines_of(copy.config / SETTINGS_FILE)) {
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
    for (std::string& line : lines_of(file)) {
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
parse_offer(const Json& release, std::string_view suffix, std::string* why) {
    Offer offer;
    offer.tag = release.get("tag_name") != nullptr ? release.get("tag_name")->string() : "";
    const Json* assets = release.get("assets");
    if (offer.tag.empty() || assets == nullptr || assets->kind != Json::Kind::Array) {
        *why = "the release has no tag or no assets";
        return std::nullopt;
    }
    if (offer.tag.starts_with('v')) {
        offer.version = offer.tag.substr(1, offer.tag.find('-') - 1);
    }
    for (const Json& asset : assets->items) {
        const std::string name = asset.get("name") != nullptr ? asset.get("name")->string() : "";
        if (!name.ends_with(suffix)) {
            continue;
        }
        const std::string digest = asset.get("digest") != nullptr ? asset.get("digest")->string() : "";
        const Json* url = asset.get("browser_download_url");
        if (!digest.starts_with(DIGEST_PREFIX) || digest.size() != std::string_view(DIGEST_PREFIX).size() + SHA256_HEX
            || url == nullptr || url->string().empty()) {
            *why = std::format("{} has no SHA-256 digest or no download URL", name);
            return std::nullopt;
        }
        offer.name = name;
        offer.url = url->string();
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
        const std::vector<std::string> said = lines_of(log);
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
    const std::optional<Json> release = parse_json(read_text(files.release));
    if (!release) {
        return std::format("{} answered no JSON", release_url);
    }
    std::optional<Offer> offer = parse_offer(*release, asset_suffix(copy), &why);
    if (!offer) {
        return why;
    }
    if (follow == Follow::Main) {
        const std::string commit_url = std::format("{}/commits/{}", API, offer->tag);
        if (!fetch(commit_url, SHA_TYPE, files.commit, &why)) {
            return std::format("cannot ask {}: {}", commit_url, why);
        }
        const std::vector<std::string> commit = lines_of(files.commit);
        offer->commit = commit.empty() ? "" : commit[0];
    }
    if (!is_newer(*offer, copy.id, follow)) {
        clear_stage(files);
        return std::format("up to date: {} offers {}", release_url, describe(*offer));
    }
    return stage(files, *offer, follow, fetch);
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
    const std::optional<std::pair<Offer, std::string>> staged = staged_offer(files);
    if (!staged) {
        return false;
    }
    const Offer& offer = staged->first;
    if (staged->second != FOLLOW_WORDS[static_cast<size_t>(follow)] || !is_newer(offer, copy.id, follow)) {
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
    // A setup runs elevated, so it runs only as hashed: no one can change the
    // file between the two.
    const bool applied = copy.id.installed
                             ? run_verified(file, offer.sha256, [&file] { return start_unwaited(file, {"/S"}); })
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
    if (!copy.exe.empty()) {
        fs::remove(fs::path(copy.exe) += ASIDE_SUFFIX, error);
    }
    if (copy.config.empty()) {
        return std::nullopt;
    }
    const Files files = files_of(copy);
    const std::vector<std::string> applied = lines_of(files.applied);
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
