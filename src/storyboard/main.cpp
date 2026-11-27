// loupe-storyboard: runs a recipe's loupe.* steps on Loupe's real window,
// Dear ImGui's test engine giving each step to the panels as a person's
// clicks, keys and typing (storyboard/runner.hpp), and writes the pictures its
// loupe.screenshot steps take under -o. The window is hidden, and keeps its
// settings and recordings under -o too, not the person's own.
#include <charconv>
#include <cstdio>
#include <map>
#include <optional>
#include <span>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "paths.hpp"
#include "storyboard/recipe.hpp"
#include "storyboard/runner.hpp"
#include "window/run_window.hpp"

namespace {

// The panels' scale when --scale is left out: the display's own is not the
// same on every machine, and a recipe's pictures should be.
constexpr float DEFAULT_SCALE = 1.0F;

// The folder under -o a run keeps its settings and recordings in.
constexpr const char CONFIG_DIR[] = "config";

struct Args {
    std::string recipe, out = ".", lock_token;
    float scale = DEFAULT_SCALE;
    std::map<std::string, std::string> names;
};

const char USAGE[] = "usage: loupe-storyboard --recipe RECIPE.yaml [-o DIR] [--scale S] [--set NAME=VALUE]...\n"
                     "                        [--lock-token TOKEN]\n";

// A flag and its value into `args`; false for a flag it does not know or a
// value it cannot take.
bool
take(Args& args, const std::pair<std::string, std::string>& given) {
    const auto& [flag, value] = given;
    if (flag == "--recipe") {
        args.recipe = value;
    } else if (flag == "-o") {
        args.out = value;
    } else if (flag == "--lock-token") {
        args.lock_token = value;
    } else if (flag == "--set") {
        const size_t equals = value.find('=');
        if (equals == std::string::npos || equals == 0) {
            return false;
        }
        args.names[value.substr(0, equals)] = value.substr(equals + 1);
    } else if (flag == "--scale") {
        const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), args.scale);
        return error == std::errc() && end == value.data() + value.size() && args.scale > 0.0F;
    } else {
        return false;
    }
    return true;
}

// The command line; none for one it cannot run.
std::optional<Args>
parse(std::span<char*> argv) {
    Args args;
    for (size_t i = 1; i + 1 < argv.size(); i += 2) {
        if (!take(args, {argv[i], argv[i + 1]})) {
            return std::nullopt;
        }
    }
    if (argv.size() % 2 == 0 || args.recipe.empty()) {
        return std::nullopt;
    }
    return args;
}

} // namespace

int
main(int argc, char** argv) {
    const std::optional<Args> args = parse(std::span(argv, static_cast<size_t>(argc)));
    if (!args) {
        std::fputs(USAGE, stderr);
        return 2;
    }
    std::string why;
    const std::filesystem::path recipe = loupe::from_utf8(args->recipe);
    std::optional<loupe::storyboard::Recipe> loaded = loupe::storyboard::load_recipe(recipe, &why);
    if (!loaded) {
        std::fprintf(stderr, "loupe-storyboard: %s\n", why.c_str());
        return 2;
    }
    const std::filesystem::path out = loupe::from_utf8(args->out);
    loupe::storyboard::Runner runner(std::move(*loaded), out, args->names, args->lock_token);
    const bool ran = loupe::run_scripted(args->scale, runner, out / CONFIG_DIR, args->lock_token);
    return ran && runner.ok() ? 0 : 1;
}
