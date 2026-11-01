#include "options.hpp"

#include <algorithm>
#include <charconv>
#include <climits>
#include <cstdint>
#include <filesystem>
#include <format>
#include <iterator>
#include <system_error>

namespace loupe {

namespace {

constexpr const char SCENE_SUFFIX[] = ".scene.toml";

// A flag that takes a whole number: its range, what an error calls it, and where it goes.
struct CountFlag {
    const char* name;
    long long least, most;
    const char* is;
    void (*set)(Options* options, long long count);
};

const CountFlag COUNT_FLAGS[] = {
    {.name = "--frames",
     .least = 1,
     .most = INT_MAX,
     .is = "a whole number from 1",
     .set = [](Options* options, long long count) { options->frames = static_cast<int>(count); }},
    {.name = "--dt",
     .least = 0,
     .most = UINT32_MAX,
     .is = "whole milliseconds",
     .set = [](Options* options, long long count) { options->dt_ms = static_cast<uint32_t>(count); }},
    {.name = "--checkpoint-mb",
     .least = 1,
     .most = LLONG_MAX / static_cast<long long>(MIB),
     .is = "whole megabytes from 1",
     .set = [](Options* options, long long count) { options->checkpoint_mb = static_cast<size_t>(count); }},
    {.name = "--ticks",
     .least = 1,
     .most = INT_MAX,
     .is = "a whole number from 1",
     .set = [](Options* options, long long count) { options->ticks = static_cast<int>(count); }},
};

const CountFlag*
find_count_flag(const std::string& flag) {
    const auto found = std::ranges::find(COUNT_FLAGS, flag, &CountFlag::name);
    return found == std::end(COUNT_FLAGS) ? nullptr : found;
}

// False with `why` set when the window's own flags come with -o or --video, which open no window.
bool
window_flags_fit(const Options& options, std::string* why) {
    const bool windowed = options.out_bmp.empty() && options.out_video.empty();
    if (!windowed && (options.checkpoint_mb || options.ticks)) {
        *why = "--checkpoint-mb and --ticks are for the window, not for -o or --video";
        return false;
    }
    return true;
}

// Sets the option `flag` names to `value`; false with `why` set for an unknown
// flag or a value it cannot take.
bool
apply_flag(const std::string& flag, const std::string& value, Options* options, std::string* why) {
    if (const CountFlag* counted = find_count_flag(flag)) {
        long long count = 0;
        if (!parse_count(value, counted->least, counted->most, &count)) {
            *why = std::format("{} is {}, not {}", flag, counted->is, value);
            return false;
        }
        counted->set(options, count);
    } else if (flag == "--object") {
        options->objects.push_back(value);
    } else if (flag == "--camera") {
        options->camera = value;
    } else if (flag == "--view") {
        options->view = value;
    } else if (flag == "--set") {
        options->sets.push_back(value);
    } else if (flag == "-o") {
        options->out_bmp = value;
    } else if (flag == "--video") {
        options->out_video = value;
    } else {
        *why = "unknown option " + flag;
        return false;
    }
    return true;
}

} // namespace

bool
parse_count(const std::string& text, long long least, long long most, long long* value) {
    const char* last = text.data() + text.size();
    const auto [end, error] = std::from_chars(text.data(), last, *value);
    return error == std::errc() && end == last && *value >= least && *value <= most;
}

std::string
scene_id(const std::string& scene_toml) {
    const std::string name = std::filesystem::path(scene_toml).filename().string();
    return name.substr(0, name.size() - (sizeof SCENE_SUFFIX - 1));
}

const char*
usage() {
    return "usage: loupe SCENE.scene.toml [--object NAME ...] [--camera NAME] [--view NAME]\n"
           "             [--frames N] [--dt MS] [--set owner.name=value ...] [-o out.bmp] [--video out.avi]\n"
           "             [--checkpoint-mb MB] [--ticks N]\n";
}

bool
parse_options(int argc, char** argv, Options* options, std::string* why) {
    for (int i = 1; i < argc; i++) {
        const std::string flag = argv[i];
        if (flag.empty() || flag[0] != '-') {
            if (!options->scene_toml.empty()) {
                *why = "one scene file only, not also " + flag;
                return false;
            }
            options->scene_toml = flag;
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
    if (!options->scene_toml.ends_with(SCENE_SUFFIX)) {
        *why =
            options->scene_toml.empty() ? "needs a scene file" : "expected a .scene.toml file: " + options->scene_toml;
        return false;
    }
    return window_flags_fit(*options, why);
}

} // namespace loupe
