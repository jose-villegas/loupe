// Self-update (src/update): the system's SHA-256 on FIPS 180-4's examples,
// as files; each copy's download found in GitHub's saved answers; which offers are newer; the setting; a check that stages
// a verified download, deletes a corrupted one and never asks for a local
// build; and a running portable copy swapped for a staged one, then settled
// at its next start.
//
// Then a staged build applies only while it is newer on the followed channel;
// a swap that fails part way puts every file back; and a setup is held
// unchangeable from its hash until it has started.
//
// usage: update_test FIXTURES OUT_DIR, which runs copies of itself as
// update_test --swap CONFIG for the swap and update_test --touch FILE.
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

#include <nlohmann/json.hpp>

#include "paths.hpp"
#include "process.hpp"
#include "update/sha256.hpp"
#include "update/update.hpp"

namespace fs = std::filesystem;
using loupe::update::Follow;

namespace {

constexpr const char TEST[] = "update_test";
constexpr const char OWN_VERSION[] = "0.1.0";
constexpr const char OWN_COMMIT[] = "aaaaaaaaa";
constexpr const char NEW_COMMIT[] = "bbbbbbbbb";
constexpr const char NEW_EXE[] = "the new build";
// The variable naming where this test, run as a setup with /S, writes its own path.
constexpr const char SETUP_OUT[] = "UPDATE_TEST_SETUP_OUT";

bool
check(bool ok, const std::string& what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, ok ? "ok" : "FAILED", what.c_str());
    return ok;
}

void
write(const fs::path& file, const std::string& text) {
    fs::create_directories(file.parent_path());
    std::ofstream(file, std::ios::binary) << text;
}

loupe::update::Copy
copy_of(loupe::Channel channel, const fs::path& exe, const fs::path& config) {
    return {.id = {.version = OWN_VERSION,
                   .commit = OWN_COMMIT,
                   .autana_commit = "262e7d0ab",
                   .built_for = channel,
                   .installed = false},
            .exe = exe,
            .config = config};
}

// GitHub stood in for by files: each URL ending in a key answers with its
// file; every request is counted.
struct FakeGitHub {
    std::vector<std::pair<std::string, fs::path>> answers;
    int asked = 0;

    loupe::update::Fetch
    fetch() {
        return [this](const std::string& url, const std::string&, const fs::path& out, std::string* why) {
            asked++;
            for (const auto& [ending, file] : answers) {
                if (url.ends_with(ending)) {
                    std::error_code error;
                    fs::create_directories(out.parent_path(), error);
                    return fs::copy_file(file, out, fs::copy_options::overwrite_existing, error);
                }
            }
            *why = "404 " + url;
            return false;
        };
    }
};

// One release offering `name` with `digest`, as GitHub's JSON.
std::string
release_json(const std::string& tag, const std::string& name, const std::string& digest) {
    return "{\"tag_name\": \"" + tag + "\", \"assets\": [{\"name\": \"" + name + "\", \"digest\": \"sha256:" + digest
           + "\", \"browser_download_url\": \"https://example.invalid/" + name + "\"}]}";
}

// The system's SHA-256 of files holding FIPS 180-4's examples.
bool
check_sha256(const fs::path& out) {
    bool ok = true;
    const std::pair<std::string, std::string> examples[] = {
        {"", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
        {"abc", "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"},
        {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
         "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"},
        {std::string(1000000, 'a'), "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"}};
#if defined(_WIN32)
    const fs::path file = out / "sha256" / "example file";
#else
    // A backslash in its name, which sha256sum escapes.
    const fs::path file = out / "sha256" / "example \\ file";
#endif
    for (const auto& [text, digest] : examples) {
        write(file, text);
        ok &= check(loupe::update::sha256_file(file) == digest, "SHA-256 of " + text.substr(0, 8) + "...");
    }
    ok &= check(loupe::update::sha256_file(out / "sha256" / "missing").empty(), "none for a missing file");
    return ok;
}

// GitHub's saved answer in `file`, parsed.
nlohmann::json
answer(const fs::path& file) {
    return nlohmann::json::parse(loupe::read_text(file), nullptr, false);
}

bool
check_offers(const fs::path& fixtures) {
    const nlohmann::json latest = answer(fixtures / "release_latest.json");
    bool ok = check(!latest.is_discarded(), "the latest release parses");
    std::string why;

    struct Want {
        const char* suffix;
        const char* name;
        const char* sha256;
    };

    for (const Want& want : {Want{.suffix = "-windows-portable.zip",
                                  .name = "loupe-0.1.0-windows-portable.zip",
                                  .sha256 = "d7be7fa69f2512717b870ce118efea06a091c60ab22694c0c7ce0ae5a3026ee1"},
                             Want{.suffix = "-windows-setup.exe",
                                  .name = "loupe-0.1.0-windows-setup.exe",
                                  .sha256 = "5c97739a21ca55dc5be30501f65b1ea8d56f383f9b8f98d090d4d437b6d4d64c"},
                             Want{.suffix = "-linux-x86_64.tar.gz",
                                  .name = "loupe-0.1.0-linux-x86_64.tar.gz",
                                  .sha256 = "e9b07b8736ff82f0be7a4348a2d8ceb771cf1c3cffa7cdb3094fc8387ed25382"}}) {
        const std::optional<loupe::update::Offer> offer = loupe::update::parse_offer(latest, want.suffix, &why);
        ok &= check(offer && offer->tag == "v0.1.0" && offer->version == "0.1.0" && offer->name == want.name
                        && offer->sha256 == want.sha256
                        && offer->url
                               == std::string("https://github.com/jose-villegas/loupe/releases/download/v0.1.0/")
                                      + want.name,
                    std::string("the latest release offers ") + want.name);
    }
    const std::optional<loupe::update::Offer> offer =
        loupe::update::parse_offer(answer(fixtures / "release_main.json"), "-windows-portable.zip", &why);
    ok &= check(offer && offer->tag == "main-build" && offer->version.empty()
                    && offer->name == "loupe-main-windows-portable.zip",
                "the main build offers its unversioned portable zip");
    ok &= check(!loupe::update::parse_offer(latest, "-macos.dmg", &why) && why.find("-macos.dmg") != std::string::npos,
                "no download for this system is refused: " + why);
    const nlohmann::json undigested =
        nlohmann::json::parse(R"({"tag_name": "v1.0.0", "assets": [{"name": "a.zip", "browser_download_url": "u"}]})");
    ok &= check(!loupe::update::parse_offer(undigested, ".zip", &why), "no digest is refused: " + why);
    const nlohmann::json odd = nlohmann::json::parse(R"({"tag_name": 1, "assets": {"name": "a.zip"}})");
    ok &= check(!loupe::update::parse_offer(odd, ".zip", &why)
                    && !loupe::update::parse_offer(nlohmann::json(), ".zip", &why),
                "fields of another type are refused, not thrown: " + why);
    return ok;
}

bool
check_newer() {
    const loupe::BuildId own = {
        .version = OWN_VERSION, .commit = OWN_COMMIT, .autana_commit = "", .built_for = loupe::Channel::Main};

    struct Case {
        const char* version;
        const char* commit;
        Follow follow;
        bool newer;
    };

    bool ok = true;
    for (const Case& c : {
             Case{.version = "0.2.0", .commit = "", .follow = Follow::Releases, .newer = true},
             Case{.version = "0.10.0", .commit = "", .follow = Follow::Releases, .newer = true},
             Case{.version = "0.1.0", .commit = "", .follow = Follow::Releases, .newer = false},
             Case{.version = "0.0.9", .commit = "", .follow = Follow::Releases, .newer = false},
             Case{.version = "garbage", .commit = "", .follow = Follow::Releases, .newer = false},
             Case{.version = "",
                  .commit = "aaaaaaaaa0123456789012345678901234567890",
                  .follow = Follow::Main,
                  .newer = false},
             Case{.version = "",
                  .commit = "bbbbbbbbb0123456789012345678901234567890",
                  .follow = Follow::Main,
                  .newer = true},
             Case{.version = "", .commit = "", .follow = Follow::Main, .newer = false},
             Case{.version = "9.0.0", .commit = "bbbbbbbbb", .follow = Follow::Off, .newer = false},
         }) {
        const loupe::update::Offer offer = {
            .tag = "", .version = c.version, .commit = c.commit, .name = "", .url = "", .sha256 = ""};
        ok &= check(loupe::update::is_newer(offer, own, c.follow) == c.newer,
                    std::string(c.version) + c.commit + (c.newer ? " is newer" : " is not newer"));
    }
    return ok;
}

bool
check_setting(const fs::path& out) {
    const fs::path config = out / "setting";
    write(config / "settings.txt", "other=1\n");
    const loupe::update::Copy main = copy_of(loupe::Channel::Main, {}, config);
    bool ok = check(loupe::update::follow(main) == Follow::Main, "a main build follows main by default");
    ok &= check(loupe::update::follow(copy_of(loupe::Channel::Releases, {}, config)) == Follow::Releases,
                "a release follows releases by default");
    ok &= check(loupe::update::set_follow(main, Follow::Off) && loupe::update::follow(main) == Follow::Off,
                "Off is kept");
    ok &= check(loupe::update::set_follow(main, Follow::Releases) && loupe::update::follow(main) == Follow::Releases,
                "Releases replaces Off");
    ok &= check(loupe::read_text(config / "settings.txt").starts_with("other=1\n"), "other settings stay");
    ok &= check(loupe::update::follow(copy_of(loupe::Channel::None, {}, config)) == Follow::Off,
                "a local build follows nothing, whatever the file says");
    return ok;
}

// The archive `name` of `top`'s folder in `dir`, by the system's tar.
bool
pack(const fs::path& dir, const std::string& top, const fs::path& archive) {
#if defined(_WIN32)
    const std::vector<std::string> how = {"-a", "-cf"};
#else
    const std::vector<std::string> how = {"-czf"};
#endif
    std::vector<std::string> args = {loupe::utf8(loupe::system_program("tar"))};
    args.insert(args.end(), how.begin(), how.end());
    args.insert(args.end(), {loupe::utf8(archive), "-C", loupe::utf8(dir), top});
    return loupe::run_logged(args, archive.parent_path() / "tar.log");
}

// A check stages only a newer, verified download, and a local build never asks.
bool
check_staging(const fs::path& out, const std::string& name, const fs::path& archive) {
    bool ok = true;
    const fs::path config = out / "staging";
    FakeGitHub github;
    const std::string digest = loupe::update::sha256_file(archive);
    write(out / "good.json", release_json("v9.9.9", name, digest));
    write(out / "old.json", release_json("v0.0.1", name, digest));
    write(out / "bad.json", release_json("v9.9.9", name, std::string(64, '0')));
    write(out / "commit.txt", std::string(NEW_COMMIT) + "0123456789012345678901234567890\n");
    github.answers = {{"/" + name, archive}};

    const loupe::update::Copy local = copy_of(loupe::Channel::None, out / "loupe", config);
    ok &= check(loupe::update::check(local, Follow::Releases, github.fetch()) == "a local build never updates"
                    && github.asked == 0,
                "a local build never asks GitHub");
    const loupe::update::Copy release = copy_of(loupe::Channel::Releases, out / "loupe", config);
    ok &= check(loupe::update::check(release, Follow::Off, github.fetch()) == "updates are off" && github.asked == 0,
                "Off never asks GitHub");

    github.answers.emplace_back("/releases/latest", out / "old.json");
    std::string said = loupe::update::check(release, Follow::Releases, github.fetch());
    ok &= check(said.starts_with("up to date") && github.asked == 1, "an older release downloads nothing: " + said);

    github.answers.back().second = out / "bad.json";
    said = loupe::update::check(release, Follow::Releases, github.fetch());
    std::string why;
    ok &= check(said.find("deleted") != std::string::npos
                    && !loupe::update::apply_staged(release, Follow::Releases, &why) && why.empty(),
                "a download that is not GitHub's digest is deleted, not staged: " + said);
    std::error_code error;
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(config, error)) {
        ok &= check(!entry.is_regular_file() || loupe::update::sha256_file(entry.path()) != digest,
                    "nothing of it is left: " + loupe::utf8(entry.path().filename()));
    }

    // The main build's commit decides; then a newer one stages, once.
    github.answers.back() = {"/releases/tags/main-build", out / "good.json"};
    github.answers.emplace_back("/commits/v9.9.9", out / "commit.txt");
    github.asked = 0;
    said = loupe::update::check(release, Follow::Main, github.fetch());
    ok &= check(said.starts_with("staged") && github.asked == 3, "another commit of main is staged: " + said);
    said = loupe::update::check(release, Follow::Main, github.fetch());
    ok &=
        check(said.find("already staged") != std::string::npos && github.asked == 5, "and not fetched again: " + said);
    return ok;
}

// The archive `name`, of a release of this copy's kind, in `dir`: the new
// executable in `bin` (empty for beside the samples) and two samples, all in
// one folder as CPack makes them.
fs::path
make_archive(const std::string& name, const fs::path& dir, const std::string& bin) {
    const fs::path top = dir / "archive" / "loupe-9.9.9";
    write(top / bin / loupe::exe_path().filename(), NEW_EXE);
    write(top / "scenes" / "a.apak", "new");
    write(top / "scenes" / "b.apak", "new");
    const fs::path archive = dir / name;
    return check(pack(dir / "archive", "loupe-9.9.9", archive), "the test archive is made in " + loupe::utf8(dir))
               ? archive
               : fs::path();
}

// A running copy of this test, its executable in `bin` (empty for its root
// folder), swapped for a staged build of the same layout.
bool
check_swap(const fs::path& out, const std::string& name, const std::string& bin) {
    const fs::path dir = out / ("swap" + bin);
    const fs::path archive = make_archive(name, dir, bin);
    const fs::path config = dir / "config";
    const fs::path root = dir / "copy";
    const fs::path exe = root / bin / loupe::exe_path().filename();
    fs::create_directories(exe.parent_path());
    fs::copy_file(loupe::exe_path(), exe, fs::copy_options::overwrite_existing);
    write(root / "scenes" / "a.apak", "old");
    const loupe::update::Copy copy = copy_of(loupe::Channel::Releases, exe, config);
    FakeGitHub github;
    write(dir / "swap.json", release_json("v9.9.9", name, loupe::update::sha256_file(archive)));
    github.answers = {{"/" + name, archive}, {"/releases/latest", dir / "swap.json"}};
    const std::string said = loupe::update::check(copy, Follow::Releases, github.fetch());
    const std::string layout = bin.empty() ? " beside its samples" : " in " + bin;
    bool ok = check(said.starts_with("staged"), "staged for the swap" + layout + ": " + said);

    ok &= check(loupe::run_logged({loupe::utf8(exe), "--swap", loupe::utf8(config)}, dir / "swap.log"),
                "the running copy" + layout + " swaps itself (" + loupe::utf8(dir / "swap.log") + ")");
    const fs::path aside = fs::path(exe) += ".old";
    ok &= check(loupe::read_text(exe) == NEW_EXE, "the new build is in its place");
    ok &= check(fs::exists(aside), "the old one is set aside");
    ok &= check(loupe::read_text(root / "scenes" / "a.apak") == "new" && fs::exists(root / "scenes" / "b.apak"),
                "the other files are the archive's, in the copy's root folder");

    loupe::update::Copy swapped = copy;
    swapped.id.commit = NEW_COMMIT;
    const std::optional<std::string> settled = loupe::update::settle(swapped);
    ok &= check(settled && *settled == std::string("Updated to ") + OWN_VERSION + " (" + NEW_COMMIT + ")",
                "the next start says it was updated: " + settled.value_or("nothing"));
    ok &= check(!fs::exists(aside), "and removes the old one");
    ok &= check(!loupe::update::settle(swapped), "once");
    return ok;
}

// A stage applies only while it is newer than the running copy on the channel
// it follows now; anything else clears it, and so does a check that finds
// nothing newer.
bool
check_stale(const fs::path& out, const std::string& name, const fs::path& archive) {
    const fs::path config = out / "stale";
    FakeGitHub github;
    write(config / "new.json", release_json("v9.9.9", name, loupe::update::sha256_file(archive)));
    write(config / "old.json", release_json("v0.0.1", name, loupe::update::sha256_file(archive)));
    github.answers = {{"/" + name, archive}, {"/releases/latest", config / "new.json"}};
    const loupe::update::Copy copy = copy_of(loupe::Channel::Releases, out / "stale" / "loupe", config);
    loupe::update::Copy updated = copy;
    updated.id.version = "9.9.9";
    std::string why;
    bool ok = true;
    // `copy` and `follow` find the stage that `check(copy, Releases)` makes not newer, so clear it.
    const auto cleared = [&](const loupe::update::Copy& running, Follow follow, const std::string& what) {
        ok &= check(loupe::update::check(copy, Follow::Releases, github.fetch()).starts_with("staged"),
                    "staged 9.9.9 for " + what);
        const bool applied = loupe::update::apply_staged(running, follow, &why);
        ok &= check(!applied && why.find("cleared") != std::string::npos, what + " does not apply it: " + why);
        ok &= check(!loupe::update::apply_staged(copy, Follow::Releases, &why) && why.empty(), "and it is gone");
    };
    cleared(updated, Follow::Releases, "a copy already at 9.9.9");
    cleared(copy, Follow::Main, "a copy that follows main now");
    ok &= check(loupe::update::check(copy, Follow::Releases, github.fetch()).starts_with("staged"), "staged again");
    github.answers.back().second = config / "old.json";
    const std::string said = loupe::update::check(copy, Follow::Releases, github.fetch());
    ok &= check(said.starts_with("up to date") && !loupe::update::apply_staged(copy, Follow::Releases, &why)
                    && why.empty(),
                "a check that finds nothing newer clears the stage: " + said);
    return ok;
}

// A swap that cannot write one of its files leaves the copy as it was: here
// the archive has a file inside zblock/, where the copy has a file zblock,
// and its other files, sorted first, are put back.
bool
check_rollback(const fs::path& out, const std::string& name) {
    const fs::path dir = out / "rollback";
    const fs::path top = dir / "archive" / "loupe-9.9.9";
    write(top / loupe::exe_path().filename(), NEW_EXE);
    write(top / "LICENSE", "new");
    write(top / "zblock" / "inner", "new");
    const fs::path archive = dir / name;
    bool ok = check(pack(dir / "archive", "loupe-9.9.9", archive), "the blocked archive is made");
    const fs::path root = dir / "copy";
    const fs::path exe = root / loupe::exe_path().filename();
    write(exe, "old exe");
    write(root / "LICENSE", "old");
    write(root / "zblock", "old");
    const loupe::update::Copy copy = copy_of(loupe::Channel::Releases, exe, dir / "config");
    FakeGitHub github;
    write(dir / "release.json", release_json("v9.9.9", name, loupe::update::sha256_file(archive)));
    github.answers = {{"/" + name, archive}, {"/releases/latest", dir / "release.json"}};
    ok &= check(loupe::update::check(copy, Follow::Releases, github.fetch()).starts_with("staged"), "staged");
    std::string why;
    const bool applied = loupe::update::apply_staged(copy, Follow::Releases, &why);
    ok &= check(!applied && !why.empty(), "the swap fails: " + why);
    ok &= check(loupe::read_text(exe) == "old exe" && loupe::read_text(root / "LICENSE") == "old"
                    && loupe::read_text(root / "zblock") == "old",
                "every file is as it was");
    std::error_code error;
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(root, error)) {
        ok &= check(entry.path().extension() != ".old", "nothing is left aside: " + loupe::utf8(entry.path()));
    }
    const bool again = loupe::update::apply_staged(copy, Follow::Releases, &why);
    ok &= check(!again && why.find("cannot") != std::string::npos, "the stage is kept for the next close: " + why);
    return ok;
}

// A file run as verified starts while held, and while held no one can write,
// rename or delete it (on Windows); a wrong digest runs nothing.
bool
check_held(const fs::path& out) {
    const fs::path dir = out / "held";
    const fs::path file = dir / loupe::exe_path().filename();
    const fs::path touched = dir / "touched";
    fs::create_directories(dir);
    fs::copy_file(loupe::exe_path(), file);
    const std::string digest = loupe::update::sha256_file(file);
    bool ok = true;
    bool ran = false;
    ok &= check(!loupe::update::run_verified(file, std::string(64, '0'), [&ran] { return ran = true; }) && !ran,
                "a wrong digest runs nothing");
    const bool started = loupe::update::run_verified(file, digest, [&] {
#if defined(_WIN32)
        std::error_code error;
        ok &= check(!std::ofstream(file, std::ios::binary | std::ios::app).is_open(), "no one can write it");
        fs::rename(file, fs::path(file) += ".moved", error);
        ok &= check(error.operator bool(), "or rename it");
        ok &= check(!fs::remove(file, error), "or delete it");
#endif
        return loupe::start_unwaited(file, {"--touch", loupe::utf8(touched)});
    });
    ok &= check(started, "the held file starts");
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (!fs::exists(touched) && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    ok &= check(fs::exists(touched), "and runs");
    return ok;
}

// A stage applies only on the build that staged it: a newer one put in by
// hand meanwhile closes without going back to it.
bool
check_staged_by(const fs::path& out, const std::string& name, const fs::path& archive) {
    const fs::path config = out / "staged_by";
    FakeGitHub github;
    write(config / "new.json", release_json("v9.9.9", name, loupe::update::sha256_file(archive)));
    github.answers = {{"/" + name, archive}, {"/releases/latest", config / "new.json"}};
    const loupe::update::Copy copy = copy_of(loupe::Channel::Releases, out / "staged_by" / "loupe", config);
    bool ok = check(loupe::update::check(copy, Follow::Releases, github.fetch()).starts_with("staged"), "staged");
    loupe::update::Copy by_hand = copy;
    by_hand.id.version = "0.2.0";
    by_hand.id.commit = NEW_COMMIT;
    std::string why;
    const bool applied = loupe::update::apply_staged(by_hand, Follow::Releases, &why);
    ok &= check(!applied && why.find("cleared") != std::string::npos,
                "a build put in by hand does not apply what another staged: " + why);
    ok &= check(!loupe::update::apply_staged(copy, Follow::Releases, &why) && why.empty(), "and it is gone");
    return ok;
}

// A swap killed part way: the next start puts every file back. The archive's
// last file is big, so the swap is still copying it when it is killed.
bool
check_crash(const fs::path& out, const std::string& name) {
    const fs::path dir = out / "crash";
    const fs::path top = dir / "archive" / "loupe-9.9.9";
    const fs::path exe_name = loupe::exe_path().filename();
    write(top / exe_name, NEW_EXE);
    write(top / "LICENSE", "new");
    {
        constexpr size_t MIB = size_t{1} << 20;
        constexpr int BIG_MIB = 1024;
        std::ofstream big(top / "zz_big", std::ios::binary);
        const std::string chunk(MIB, 'x');
        for (int i = 0; i < BIG_MIB; i++) {
            big << chunk;
        }
    }
    const fs::path archive = dir / name;
    bool ok = check(pack(dir / "archive", "loupe-9.9.9", archive), "the big archive is made");
    fs::remove_all(dir / "archive");
    const fs::path root = dir / "copy";
    const fs::path exe = root / exe_name;
    write(exe, "old exe");
    write(root / "LICENSE", "old");
    const fs::path config = dir / "config";
    const loupe::update::Copy copy = copy_of(loupe::Channel::Releases, exe, config);
    FakeGitHub github;
    write(dir / "release.json", release_json("v9.9.9", name, loupe::update::sha256_file(archive)));
    github.answers = {{"/" + name, archive}, {"/releases/latest", dir / "release.json"}};
    ok &= check(loupe::update::check(copy, Follow::Releases, github.fetch()).starts_with("staged"), "staged");

    std::stop_source stop;
    bool finished = false;
    std::jthread swap([&] {
        finished = loupe::run_logged({loupe::utf8(loupe::exe_path()), "--apply", loupe::utf8(config), loupe::utf8(exe)},
                                     dir / "apply.log", stop.get_token());
    });
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(120);
    while (!fs::exists(root / "zz_big") && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    stop.request_stop();
    swap.join();
    ok &= check(!finished && loupe::read_text(exe) == NEW_EXE, "the swap is killed part way, the new build in place");

    (void)loupe::update::settle(copy);
    ok &= check(loupe::read_text(exe) == "old exe" && loupe::read_text(root / "LICENSE") == "old"
                    && !fs::exists(root / "zz_big"),
                "the next start puts every file back");
    std::error_code error;
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(root, error)) {
        ok &= check(entry.path().filename() == exe_name || entry.path().filename() == "LICENSE",
                    "and leaves nothing else: " + loupe::utf8(entry.path()));
    }
    fs::remove_all(dir, error);
    return ok;
}

#if defined(_WIN32)
// The setup a check staged runs from a new folder of its own, nothing beside it.
bool
check_setup_folder(const fs::path& out) {
    const fs::path dir = out / "setup";
    loupe::update::Copy copy = copy_of(loupe::Channel::Releases, dir / "loupe" / "loupe.exe", dir / "config");
    copy.id.installed = true;
    const std::string name = "loupe-9.9.9" + std::string(loupe::update::asset_suffix(copy));
    const fs::path setup = dir / name;
    fs::create_directories(dir);
    fs::copy_file(loupe::exe_path(), setup);
    FakeGitHub github;
    write(dir / "release.json", release_json("v9.9.9", name, loupe::update::sha256_file(setup)));
    github.answers = {{"/" + name, setup}, {"/releases/latest", dir / "release.json"}};
    bool ok = check(loupe::update::check(copy, Follow::Releases, github.fetch()).starts_with("staged"), "staged");
    const fs::path ran = dir / "ran.txt";
    _putenv_s(SETUP_OUT, loupe::utf8(ran).c_str());
    std::string why;
    ok &= check(loupe::update::apply_staged(copy, Follow::Releases, &why), "the setup starts: " + why);
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (!fs::exists(ran) && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    const fs::path folder = loupe::from_utf8(loupe::read_text(ran)).parent_path();
    std::error_code error;
    const auto beside = std::distance(fs::directory_iterator(folder, error), fs::directory_iterator());
    ok &= check(!folder.empty() && !fs::equivalent(folder, dir / "config" / "update" / "staged", error) && beside == 1,
                "from a folder of its own, nothing beside it: " + loupe::utf8(folder));
    return ok;
}
#endif

// The child: this process, a staged copy's running executable, swaps itself.
int
swap_self(const fs::path& config) {
    std::string why;
    const bool swapped = loupe::update::apply_staged(copy_of(loupe::Channel::Releases, loupe::exe_path(), config),
                                                     Follow::Releases, &why);
    std::fprintf(stderr, "%s --swap: %s\n", TEST, swapped ? "swapped" : why.c_str());
    return swapped ? 0 : 1;
}

} // namespace

int
main(int argc, char** argv) {
    if (argc == 3 && std::string(argv[1]) == "--swap") {
        return swap_self(loupe::from_utf8(argv[2]));
    }
    if (argc == 4 && std::string(argv[1]) == "--apply") {
        std::string why;
        const loupe::update::Copy copy =
            copy_of(loupe::Channel::Releases, loupe::from_utf8(argv[3]), loupe::from_utf8(argv[2]));
        return loupe::update::apply_staged(copy, Follow::Releases, &why) ? 0 : 1;
    }
    if (argc == 2 && std::string(argv[1]) == "/S") {
        const char* ran = std::getenv(SETUP_OUT);
        write(loupe::from_utf8(ran != nullptr ? ran : ""), loupe::utf8(loupe::exe_path()));
        return 0;
    }
    if (argc == 3 && std::string(argv[1]) == "--touch") {
        write(loupe::from_utf8(argv[2]), "");
        return 0;
    }
    if (argc != 3) {
        std::fprintf(stderr, "usage: %s FIXTURES OUT_DIR\n", TEST);
        return 2;
    }
    const fs::path fixtures = loupe::from_utf8(argv[1]);
    const fs::path out = fs::absolute(loupe::from_utf8(argv[2]));
    fs::remove_all(out);

    const loupe::update::Copy kind = copy_of(loupe::Channel::Releases, {}, {});
    const std::string name = "loupe-9.9.9" + std::string(loupe::update::asset_suffix(kind));
    const fs::path archive = make_archive(name, out / "staging_archive", "");
    bool ok = !archive.empty();
    ok &= check_sha256(out);
    ok &= check_offers(fixtures);
    ok &= check_newer();
    ok &= check_setting(out);
    ok &= check_staging(out, name, archive);
    ok &= check_swap(out, name, "");
    ok &= check_swap(out, name, "bin");
    ok &= check_stale(out, name, archive);
    ok &= check_rollback(out, name);
    ok &= check_held(out);
    ok &= check_staged_by(out, name, archive);
    ok &= check_crash(out, name);
#if defined(_WIN32)
    ok &= check_setup_folder(out);
#endif
    return ok ? 0 : 1;
}
