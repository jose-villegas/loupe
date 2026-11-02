#include "options.hpp"

#include <algorithm>
#include <charconv>
#include <climits>
#include <cstdint>
#include <filesystem>
#include <format>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "pack.hpp"
#include "paths.hpp"
#include "tools.hpp"

namespace loupe {

namespace {

// A flag that takes a whole number: its range, what an error calls it, and where it goes.
struct CountFlag {
    const char* name;
    long long least, most;
    const char* is;
    void (*set)(Options* options, long long count);
};

// Sets the optional<T> `field` of the options to a count already in T's range.
template <typename T, std::optional<T> Options::* field>
void
set_count(Options* options, long long count) {
    options->*field = static_cast<T>(count);
}

const CountFlag COUNT_FLAGS[] = {
    {.name = "--frames",
     .least = 1,
     .most = INT_MAX,
     .is = "a whole number from 1",
     .set = set_count<int, &Options::frames>},
    {.name = "--dt",
     .least = 0,
     .most = UINT32_MAX,
     .is = "whole milliseconds",
     .set = set_count<uint32_t, &Options::dt_ms>},
    {.name = "--checkpoint-mb",
     .least = 1,
     .most = LLONG_MAX / static_cast<long long>(MIB),
     .is = "whole megabytes from 1",
     .set = set_count<size_t, &Options::checkpoint_mb>},
    {.name = "--ticks",
     .least = 1,
     .most = INT_MAX,
     .is = "a whole number from 1",
     .set = set_count<int, &Options::ticks>},
    {.name = "--frame",
     .least = 0,
     .most = INT_MAX,
     .is = "a frame index from 0",
     .set = set_count<int, &Options::frame>},
};

// `text` split at white space.
std::vector<std::string>
words(const std::string& text) {
    std::istringstream in(text);
    return {std::istream_iterator<std::string>(in), std::istream_iterator<std::string>()};
}

const CountFlag*
find_count_flag(const std::string& flag) {
    const auto found = std::ranges::find(COUNT_FLAGS, flag, &CountFlag::name);
    return found == std::end(COUNT_FLAGS) ? nullptr : found;
}

// False with `why` set when -o or --video come without a scene, or with the
// window's own flags: they open no window.
bool
window_flags_fit(const Options& options, std::string* why) {
    const bool windowed = options.out_bmp.empty() && options.out_video.empty();
    if (!windowed && options.scene.empty()) {
        *why = "-o and --video need a scene file";
        return false;
    }
    if (!windowed && (options.checkpoint_mb || options.ticks)) {
        *why = "--checkpoint-mb and --ticks are for the window, not for -o or --video";
        return false;
    }
    return true;
}

// False with `why` set when --frame, --compare or --b-args come without what
// they need: --frame writes -o, a compare writes --video or --frame's pair,
// and B's pack is built from the .scene.toml.
bool
path_flags_fit(const Options& options, std::string* why) {
    if (options.b_render && options.compare.empty()) {
        *why = "--b-args needs --compare REV|DIR";
        return false;
    }
    if (options.frame && (options.out_bmp.empty() || !options.out_video.empty())) {
        *why = "--frame N writes -o OUT.png, and no --video";
        return false;
    }
    if ((options.frame || !options.compare.empty()) && options.dt_ms == 0U) {
        *why = "--frame and --compare time the path at a nonzero --dt";
        return false;
    }
    if (options.compare.empty()) {
        return true;
    }
    if (!builds_packs()) {
        *why = "this loupe was built without the pack tool --compare needs to build B's pack";
        return false;
    }
    if (!options.frame && (options.out_video.empty() || !options.out_bmp.empty())) {
        *why = "--compare writes --video OUT.mp4, or --frame N -o OUT.png";
        return false;
    }
    if (!utf8(options.scene.filename()).ends_with(SCENE_SUFFIX)) {
        *why = "--compare needs a .scene.toml: B's pack is built from it";
        return false;
    }
    return true;
}

// --b-args' words as side B's renderer flags; false with `why` set for a word
// that is not a renderer flag, or one without its value.
bool
parse_b_args(const std::string& text, RenderFlags* flags, std::string* why) {
    const std::vector<std::string> given = words(text);
    for (size_t i = 0; i < given.size(); i += 2) {
        if (i + 1 == given.size()) {
            *why = "--b-args: " + given[i] + " needs a value";
            return false;
        }
        if (!flags->set(given[i], given[i + 1])) {
            *why = "--b-args takes --object, --camera, --view and --set, not " + given[i];
            return false;
        }
    }
    return true;
}

// Sets the option `flag` names to `value`; false with `why` set for an unknown
// flag or a value it cannot take.
bool
apply_flag(const std::string& flag, const std::string& value, Options* options, std::string* why) {
    if (options->render.set(flag, value)) {
        return true;
    }
    if (const CountFlag* counted = find_count_flag(flag)) {
        long long count = 0;
        if (!parse_count(value, counted->least, counted->most, &count)) {
            *why = std::format("{} is {}, not {}", flag, counted->is, value);
            return false;
        }
        counted->set(options, count);
    } else if (flag == "-o") {
        options->out_bmp = value;
    } else if (flag == "--video") {
        options->out_video = value;
    } else if (flag == "--compare") {
        options->compare = value;
    } else if (flag == "--b-args") {
        return parse_b_args(value, &options->b_render.emplace(), why);
    } else {
        *why = "unknown option " + flag;
        return false;
    }
    return true;
}

} // namespace

bool
RenderFlags::set(std::string_view flag, const std::string& value) {
    if (flag == "--object") {
        objects.push_back(value);
    } else if (flag == "--camera") {
        camera = value;
    } else if (flag == "--view") {
        view = value;
    } else if (flag == "--set") {
        sets.push_back(value);
    } else {
        return false;
    }
    return true;
}

std::vector<std::string>
RenderFlags::args() const {
    std::vector<std::string> args;
    const auto add = [&args](const char* flag, const std::string& value) {
        if (!value.empty()) {
            args.insert(args.end(), {flag, value});
        }
    };
    for (const std::string& object : objects) {
        add("--object", object);
    }
    add("--camera", camera);
    add("--view", view);
    for (const std::string& assignment : sets) {
        add("--set", assignment);
    }
    return args;
}

bool
parse_count(const std::string& text, long long least, long long most, long long* value) {
    const char* last = text.data() + text.size();
    const auto [end, error] = std::from_chars(text.data(), last, *value);
    return error == std::errc() && end == last && *value >= least && *value <= most;
}

const char*
usage() {
    return "usage: loupe [SCENE.apak | SCENE.scene.toml] [--object NAME ...] [--camera NAME] [--view NAME]\n"
           "             [--frames N] [--dt MS] [--set owner.name=value ...] [-o out.bmp] [--video out.avi]\n"
           "             [--checkpoint-mb MB] [--ticks N]\n"
           "       loupe SCENE [flags] [--frames N] [--dt MS] --frame N -o out.png\n"
           "       loupe SCENE.scene.toml [flags] [--frames N] [--dt MS] --compare REV|DIR [--b-args \"FLAGS\"]\n"
           "             (--video out.mp4 | --frame N -o out.png)\n"
           "       loupe --version\n";
}

bool
parse_options(int argc, char** argv, Options* options, std::string* why) {
    for (int i = 1; i < argc; i++) {
        const std::string flag = argv[i];
        if (flag.empty() || flag[0] != '-') {
            if (!options->scene.empty()) {
                *why = "one scene file only, not also " + flag;
                return false;
            }
            options->scene = flag;
            continue;
        }
        if (flag == "--version") {
            options->version = true;
            continue;
        }
        if (i + 1 >= argc) {
            *why = flag + " needs a value";
            return false;
        }
        if (!apply_flag(flag, argv[++i], options, why)) {
            return false;
        }
    }
    return window_flags_fit(*options, why) && path_flags_fit(*options, why);
}

} // namespace loupe
