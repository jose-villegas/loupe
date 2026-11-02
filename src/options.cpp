#include "options.hpp"

#include <algorithm>
#include <charconv>
#include <climits>
#include <cstdint>
#include <filesystem>
#include <format>
#include <iterator>
#include <optional>
#include <string>
#include <system_error>
#include <vector>

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
};

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

const char*
usage() {
    return "usage: loupe [SCENE.apak | SCENE.scene.toml] [--object NAME ...] [--camera NAME] [--view NAME]\n"
           "             [--frames N] [--dt MS] [--set owner.name=value ...] [-o out.bmp] [--video out.avi]\n"
           "             [--checkpoint-mb MB] [--ticks N]\n";
}

std::vector<std::string>
window_args(const std::filesystem::path& exe, const std::filesystem::path& scene, std::optional<size_t> checkpoint_mb) {
    std::vector<std::string> args = {exe.string(), scene.string()};
    if (checkpoint_mb) {
        args.insert(args.end(), {"--checkpoint-mb", std::to_string(*checkpoint_mb)});
    }
    return args;
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
        if (i + 1 >= argc) {
            *why = flag + " needs a value";
            return false;
        }
        if (!apply_flag(flag, argv[++i], options, why)) {
            return false;
        }
    }
    return window_flags_fit(*options, why);
}

} // namespace loupe
