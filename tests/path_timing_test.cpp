// The path's timing follows the scene viewer's rule: the run's own frame time
// over the whole path; when that is more frames than one video holds, the
// shortest whole millisecond at which the path fits; --frames and --dt as given.
#include <cstdint>
#include <cstdio>

#include "options.hpp"
#include "path/path.hpp"

namespace {

constexpr const char TEST[] = "path_timing_test";
// Sponza's tour at the scene viewer's 16 ms, and how many of its 448x368 frames one video holds.
constexpr loupe::PathFacts TOUR = {.path_ms = 36303, .own_dt_ms = 16, .fit = 2170};

struct Case {
    const char* what;
    loupe::PathFacts path;
    loupe::Options given;
    loupe::PathTiming want;
};

loupe::Options
given(std::optional<int> frames, std::optional<uint32_t> dt_ms) {
    loupe::Options options;
    options.frames = frames;
    options.dt_ms = dt_ms;
    return options;
}

bool
check(const Case& c) {
    const loupe::PathTiming got = loupe::path_timing(c.path, c.given);
    const bool ok = got.frames == c.want.frames && got.dt_ms == c.want.dt_ms;
    std::fprintf(stderr, "%s: %s: %d frames at %u ms %s\n", TEST, c.what, got.frames, got.dt_ms, ok ? "ok" : "FAILED");
    return ok;
}

} // namespace

int
main() {
    // 36303 / 16 + 1 = 2269 frames is past the fit: 36303 / (2170 - 1) rounded up is 17 ms, 2136 frames.
    const Case cases[] = {
        {.what = "a path past one video", .path = TOUR, .given = given({}, {}), .want = {.frames = 2136, .dt_ms = 17}},
        {.what = "a path one video holds",
         .path = {.path_ms = TOUR.path_ms, .own_dt_ms = TOUR.own_dt_ms, .fit = 3000},
         .given = given({}, {}),
         .want = {.frames = 2269, .dt_ms = 16}},
        {.what = "--dt kept past one video", .path = TOUR, .given = given({}, 7), .want = {.frames = 5187, .dt_ms = 7}},
        {.what = "--frames kept, the fitted dt",
         .path = TOUR,
         .given = given(3, {}),
         .want = {.frames = 3, .dt_ms = 17}},
        {.what = "--frames and --dt kept", .path = TOUR, .given = given(3, 7), .want = {.frames = 3, .dt_ms = 7}},
        {.what = "no path: one frame",
         .path = {.path_ms = 0, .own_dt_ms = TOUR.own_dt_ms, .fit = TOUR.fit},
         .given = given({}, {}),
         .want = {.frames = 1, .dt_ms = 16}},
    };
    bool ok = true;
    for (const Case& c : cases) {
        ok &= check(c);
    }
    return ok ? 0 : 1;
}
