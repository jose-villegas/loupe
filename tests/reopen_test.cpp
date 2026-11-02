// Whether autana's run can be closed and opened again in one process as if
// fresh. Each pack named is opened in turn and stepped through its run; a
// pack named again must draw every frame as it did the first time. One after
// REFUSED must fail to open, and leave the next opening unharmed.
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <format>
#include <map>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "frames_fixture.hpp"

namespace {

constexpr const char TEST[] = "reopen_test";
constexpr const char REFUSED[] = "--refused";
// Fewer than either scene's own count, so each run is cut short at the same frame every time.
constexpr int FRAMES = 40;

using Drawn = std::vector<std::vector<uint8_t>>;

// Every frame of `pack`'s run, its last BMP written to `out_bmp`; empty when the run failed.
Drawn
run(const std::filesystem::path& pack, const std::string& out_bmp) {
    loupe::Options options;
    options.scene = pack;
    options.frames = FRAMES;
    options.out_bmp = out_bmp;
    std::unique_ptr<loupe::Frames> frames = loupe::test::open_or_report(TEST, options);
    if (frames == nullptr) {
        return {};
    }
    Drawn drawn;
    bool ok = true;
    while (frames->index() < frames->count()) {
        ok &= frames->step(frames->dt_ms());
        drawn.push_back(loupe::test::pixels(*frames));
    }
    ok &= frames->close();
    return ok ? drawn : Drawn{};
}

// Keeps `pack`'s first opening in `first`; a later one must draw the same, said on stderr.
bool
same_as_first(std::map<std::string, Drawn>& first, const std::string& pack, Drawn drawn) {
    const auto seen = first.find(pack);
    if (seen == first.end()) {
        first.emplace(pack, std::move(drawn));
        return true;
    }
    const bool same = seen->second == drawn;
    std::fprintf(stderr, "%s: %s opened again %s\n", TEST, pack.c_str(), same ? "same" : "DIFFERS");
    return same;
}

} // namespace

int
main(int argc, char** argv) {
    const std::span<char*> args(argv, static_cast<size_t>(argc));
    if (args.size() < 3) {
        std::fprintf(stderr, "usage: %s OUT_DIR [%s] SCENE.apak ...\n", TEST, REFUSED);
        return 2;
    }
    const std::string out = args[1];
    std::filesystem::create_directories(out);
    std::map<std::string, Drawn> first;
    bool ok = true;
    bool refused = false;
    for (size_t i = 2; i < args.size(); i++) {
        const std::string pack = args[i];
        if (pack == REFUSED) {
            refused = true;
            continue;
        }
        Drawn drawn = run(pack, std::format("{}/{}.bmp", out, i - 2));
        if (drawn.empty() != refused) {
            std::fprintf(stderr, "%s: %s %s\n", TEST, pack.c_str(), refused ? "opened, not refused" : "failed");
            ok = false;
        }
        if (!std::exchange(refused, false) && !drawn.empty()) {
            ok &= same_as_first(first, pack, std::move(drawn));
        }
    }
    return ok ? 0 : 1;
}
