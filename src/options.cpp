#include "options.h"

#include <cstdlib>

namespace loupe {

namespace {

constexpr const char SCENE_SUFFIX[] = ".scene.toml";

bool ends_with(const std::string& text, const std::string& suffix) {
    return text.size() >= suffix.size() && text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// A whole decimal number at least `least`; false for anything else.
bool parse_count(const std::string& text, long least, long* value) {
    char* end = nullptr;
    *value = std::strtol(text.c_str(), &end, 10);
    return !text.empty() && *end == '\0' && *value >= least;
}

}  // namespace

const char* usage() {
    return "usage: loupe SCENE.scene.toml [--object NAME ...] [--camera NAME] [--view NAME]\n"
           "             [--frames N] [--dt MS] [--set owner.name=value ...] [-o out.bmp] [--video out.avi]\n";
}

bool parse_options(int argc, char** argv, Options* options, std::string* why) {
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
        const std::string value = argv[++i];
        long count = 0;
        if (flag == "--object") {
            options->objects.push_back(value);
        } else if (flag == "--camera") {
            options->camera = value;
        } else if (flag == "--view") {
            options->view = value;
        } else if (flag == "--frames") {
            if (!parse_count(value, 1, &count)) {
                *why = "--frames is a whole number from 1, not " + value;
                return false;
            }
            options->frames = static_cast<int>(count);
        } else if (flag == "--dt") {
            if (!parse_count(value, 0, &count)) {
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
    }
    if (!ends_with(options->scene_toml, SCENE_SUFFIX)) {
        *why = options->scene_toml.empty() ? "needs a scene file" : "expected a .scene.toml file: " + options->scene_toml;
        return false;
    }
    return true;
}

}  // namespace loupe
