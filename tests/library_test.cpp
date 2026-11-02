// The start panel's library: the sample packs in the folder they install to,
// and a recent list that keeps the last RECENT_KEPT scenes opened, most
// recent first, each once, without the files gone since; none without a config folder.
#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <vector>

#include "library.hpp"

namespace fs = std::filesystem;

namespace {

constexpr const char TEST[] = "library_test";

void
touch(const fs::path& file) {
    fs::create_directories(file.parent_path());
    std::ofstream(file, std::ios::binary).put('\0');
}

bool
check(bool ok, const char* what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what, ok ? "ok" : "FAILED");
    return ok;
}

} // namespace

int
main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s OUT_DIR\n", TEST);
        return 2;
    }
    const fs::path out = fs::absolute(argv[1]);
    fs::remove_all(out);
    const loupe::Folders folders = {.exe_dir = out / "bin", .config = out / "config"};
    const fs::path first = loupe::samples_dir(folders) / "a.apak";
    const fs::path second = loupe::samples_dir(folders) / "b.apak";
    touch(second);
    touch(first);
    touch(loupe::samples_dir(folders) / "notes.txt");

    // One more scene than the list keeps, opened in order, then the first two again.
    std::vector<fs::path> scenes;
    for (size_t i = 0; i <= loupe::RECENT_KEPT; i++) {
        scenes.push_back(out / "scenes" / std::format("s{}.apak", i));
        touch(scenes.back());
    }
    bool ok = true;
    for (const fs::path& scene : scenes) {
        ok &= loupe::remember(folders, scene);
    }
    ok &= loupe::remember(folders, scenes[1]);
    ok &= loupe::remember(folders, out / "scenes" / ".." / "scenes" / "s2.apak");
    fs::remove(scenes[3]);

    const loupe::Library found = loupe::library(folders);
    ok &= check(found.samples.size() == 2 && fs::equivalent(found.samples[0], first)
                    && fs::equivalent(found.samples[1], second),
                "samples, by name, packs only");
    // s2, s1, then s10 down to s4: s0 fell off the end, s3 is gone, and s1 and s2 moved up.
    std::vector<fs::path> expected = {scenes[2], scenes[1]};
    for (size_t i = loupe::RECENT_KEPT; i >= 4; i--) {
        expected.push_back(scenes[i]);
    }
    ok &= check(found.recent == expected, "recent, most recent first, each once, without the gone");
    // Without a config folder, nothing is remembered, and nothing lands in the working directory.
    const loupe::Folders none = {.exe_dir = folders.exe_dir, .config = {}};
    ok &= check(!loupe::remember(none, scenes[1]) && loupe::library(none).recent.empty(), "no config folder");
    return ok ? 0 : 1;
}
