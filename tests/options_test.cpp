// --frame, --compare and --b-args parse only together with what they need,
// each refusal saying why, and side B takes --b-args' renderer flags in place
// of A's while keeping the rest.
#include <cstdio>
#include <string>
#include <vector>

#include "options.hpp"
#include "pack.hpp"

namespace {

constexpr const char TEST[] = "options_test";

struct Case {
    std::vector<std::string> args; // after the program name
    // Empty when the command line parses; else what its refusal says.
    std::string refusal;
};

// `args` parsed into `options`; the refusal, or empty when they parse.
std::string
parse(const std::vector<std::string>& args, loupe::Options* options) {
    std::vector<std::string> line = {"loupe"};
    line.insert(line.end(), args.begin(), args.end());
    std::vector<char*> argv;
    argv.reserve(line.size());
    for (std::string& arg : line) {
        argv.push_back(arg.data());
    }
    std::string why;
    return loupe::parse_options(static_cast<int>(argv.size()), argv.data(), options, &why) ? "" : why;
}

bool
check(bool ok, const std::string& what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, ok ? "ok" : "FAILED", what.c_str());
    return ok;
}

// Says whether `c` parses, or is refused with its reason.
bool
check_case(const Case& c) {
    std::string line = "loupe";
    for (const std::string& arg : c.args) {
        line += " " + arg;
    }
    loupe::Options options;
    const std::string refusal = parse(c.args, &options);
    return check(c.refusal.empty() ? refusal.empty() : refusal.find(c.refusal) != std::string::npos,
                 line + (refusal.empty() ? ": parses" : ": " + refusal));
}

// Each command line parses, or is refused for its reason.
bool
check_refusals() {
    const std::string toml = "s.scene.toml";
    const bool tool = loupe::builds_packs();
    const std::string no_tool = "built without the pack tool";
    const std::vector<Case> cases = {
        {.args = {"s.apak", "--frame", "2", "-o", "f.png"}, .refusal = ""},
        {.args = {"s.apak", "--frame", "2"}, .refusal = "--frame N writes -o"},
        {.args = {"s.apak", "--frame", "2", "-o", "f.png", "--video", "v.avi"}, .refusal = "--frame N writes -o"},
        {.args = {"s.apak", "--frame", "-1", "-o", "f.png"}, .refusal = "--frame is a frame index from 0"},
        {.args = {"s.apak", "--dt", "0", "--frame", "1", "-o", "f.png"}, .refusal = "at a nonzero --dt"},
        {.args = {toml, "--b-args", "--view meshlets", "--video", "v.mp4"}, .refusal = "--b-args needs --compare"},
        {.args = {toml, "--compare", "HEAD", "--video", "v.mp4"}, .refusal = tool ? "" : no_tool},
        {.args = {toml, "--compare", "HEAD", "--frame", "1", "-o", "f.png"}, .refusal = tool ? "" : no_tool},
        {.args = {toml, "--compare", "HEAD", "-o", "f.png"}, .refusal = tool ? "--compare writes --video" : no_tool},
        {.args = {toml, "--compare", "HEAD", "--video", "v.mp4", "-o", "f.png"},
         .refusal = tool ? "--compare writes --video" : no_tool},
        {.args = {"s.apak", "--compare", "HEAD", "--video", "v.mp4"},
         .refusal = tool ? "--compare needs a .scene.toml" : no_tool},
        {.args = {toml, "--compare", "HEAD", "--b-args", "--frames 3", "--video", "v.mp4"},
         .refusal = tool ? "--b-args takes --object, --camera, --view and --set, not --frames" : no_tool},
        {.args = {toml, "--compare", "HEAD", "--b-args", "--view", "--video", "v.mp4"},
         .refusal = tool ? "--b-args: --view needs a value" : no_tool},
    };
    bool ok = true;
    for (const Case& c : cases) {
        ok &= check_case(c);
    }
    return ok;
}

// B takes --b-args' renderer flags in place of A's, and keeps the path and the output.
bool
check_b_options() {
    const std::string toml = "s.scene.toml";
    const bool tool = loupe::builds_packs();
    bool ok = true;
    loupe::Options a;
    loupe::Options b;
    std::string why;
    const std::vector<std::string> both = {toml,
                                           "--object",
                                           "atrium",
                                           "--view",
                                           "shaded",
                                           "--set",
                                           "a.b=1",
                                           "--dt",
                                           "50",
                                           "--frame",
                                           "2",
                                           "-o",
                                           "f.png",
                                           "--compare",
                                           "HEAD",
                                           "--b-args",
                                           "  --view meshlets\t--set c.d=2 "};
    // Parsed in builds without the pack tool too, though they refuse the compare.
    ok &= check(parse(both, &a).empty() == tool, "A's compare command line");
    ok &= check(loupe::b_options(a, &b, &why), "B's options build");
    ok &= check(b.objects.empty() && b.camera.empty() && b.view == "meshlets"
                    && b.sets == std::vector<std::string>{"c.d=2"},
                "B takes --b-args' renderer flags in place of A's");
    ok &= check(b.dt_ms == a.dt_ms && b.frame == a.frame && b.out_bmp == a.out_bmp && b.scene == a.scene,
                "B keeps A's path, frame, output and scene");
    a.b_args.reset();
    ok &= check(loupe::b_options(a, &b, &why) && b.objects == a.objects && b.view == a.view && b.sets == a.sets,
                "without --b-args, B draws as A");
    return ok;
}

} // namespace

int
main() {
    const bool refusals = check_refusals();
    const bool b_options = check_b_options();
    return refusals && b_options ? 0 : 1;
}
