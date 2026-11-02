// The command line the window starts loupe again with, for a scene picked
// after another, parses back to a window on that scene with the provider's
// defaults and the checkpoint budget it was given, if any.
#include <cstddef>
#include <cstdio>
#include <optional>
#include <string>
#include <vector>

#include "options.hpp"

namespace {

constexpr const char TEST[] = "options_test";
constexpr const char PICKED[] = "picked dir/second \"scene\".apak";
constexpr size_t BUDGET_MB = 256;

bool
check(bool ok, const std::string& what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what.c_str(), ok ? "ok" : "FAILED");
    return ok;
}

// The window_args() for `budget` parsed back; false when they do not parse.
bool
parse_back(std::optional<size_t> budget, loupe::Options* parsed) {
    const std::vector<std::string> args = loupe::window_args("loupe", PICKED, budget);
    std::vector<char*> argv;
    argv.reserve(args.size());
    for (const std::string& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    std::string why;
    return loupe::parse_options(static_cast<int>(argv.size()), argv.data(), parsed, &why);
}

} // namespace

int
main() {
    bool ok = true;
    for (const std::optional<size_t> budget : {std::optional<size_t>(), std::optional<size_t>(BUDGET_MB)}) {
        const std::string with = budget ? "with a budget" : "without a budget";
        loupe::Options parsed;
        ok &= check(parse_back(budget, &parsed), with + ": parses");
        ok &= check(parsed.scene == PICKED && parsed.checkpoint_mb == budget, with + ": the scene and budget");
        ok &= check(parsed.objects.empty() && parsed.camera.empty() && parsed.view.empty() && !parsed.frames
                        && !parsed.dt_ms && parsed.sets.empty() && !parsed.ticks && parsed.out_bmp.empty()
                        && parsed.out_video.empty(),
                    with + ": nothing else");
    }
    return ok ? 0 : 1;
}
