// loupe-storyboard: runs a recipe's loupe.* steps on Loupe's real window,
// Dear ImGui's test engine giving each step to the panels as a person's
// clicks, keys and typing (storyboard/runner.hpp), and writes the pictures its
// loupe.screenshot steps take under -o.
#include <charconv>
#include <cstdio>
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

struct Args {
    std::string recipe, out = ".";
    float scale = DEFAULT_SCALE;
};

const char USAGE[] = "usage: loupe-storyboard --recipe RECIPE.yaml [-o DIR] [--scale S]\n";

// The command line; none for one it cannot run.
std::optional<Args>
parse(std::span<char*> argv) {
    Args args;
    for (size_t i = 1; i + 1 < argv.size(); i += 2) {
        const std::string flag = argv[i];
        const std::string value = argv[i + 1];
        if (flag == "--recipe") {
            args.recipe = value;
        } else if (flag == "-o") {
            args.out = value;
        } else if (flag == "--scale") {
            const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), args.scale);
            if (error != std::errc() || end != value.data() + value.size() || args.scale <= 0.0F) {
                return std::nullopt;
            }
        } else {
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
    loupe::storyboard::Runner runner(std::move(*loaded), loupe::from_utf8(args->out));
    const bool ran = loupe::run_scripted(args->scale, runner);
    return ran && runner.ok() ? 0 : 1;
}
