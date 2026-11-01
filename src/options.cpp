#include "options.hpp"

#include <charconv>
#include <climits>
#include <cstdint>
#include <filesystem>
#include <system_error>

namespace loupe {

namespace {

constexpr const char SCENE_SUFFIX[] = ".scene.toml";

// Sets the option `flag` names to `value`; false with `why` set for an unknown
// flag or a value it cannot take.
bool
apply_flag(const std::string& flag, const std::string& value, Options* options, std::string* why) {
    long long count = 0;
    if (flag == "--object") {
        options->objects.push_back(value);
    } else if (flag == "--camera") {
        options->camera = value;
    } else if (flag == "--view") {
        options->view = value;
    } else if (flag == "--frames") {
        if (!parse_count(value, 1, INT_MAX, &count)) {
            *why = "--frames is a whole number from 1, not " + value;
            return false;
        }
        options->frames = static_cast<int>(count);
    } else if (flag == "--dt") {
        if (!parse_count(value, 0, UINT32_MAX, &count)) {
            *why = "--dt is whole milliseconds, not " + value;
            return false;
        }
        options->dt_ms = static_cast<uint32_t>(count);
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
           "             [--frames N] [--dt MS] [--set owner.name=value ...] [-o out.bmp] [--video out.avi]\n";
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
    return true;
}

} // namespace loupe
