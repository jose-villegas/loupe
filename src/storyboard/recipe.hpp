// A recipe: the YAML list of steps loupe-storyboard runs, each a map of one
// "loupe." key, its value the step's arguments, and an optional name.
#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace loupe::storyboard {

// The prefix of Loupe's step keys.
constexpr const char LOUPE_KEY_PREFIX[] = "loupe.";

// One step: its map as JSON, its "loupe.*" key, and the recipe line it starts on (1-based).
struct Item {
    nlohmann::json raw;
    std::string key;
    int line = 0;

    // The step's label: its name, or its key.
    std::string label() const;
};

// A recipe file: where it is, which its loupe.open paths are relative to, and its steps.
struct Recipe {
    std::filesystem::path path;
    std::vector<Item> steps;
};

// The recipe at `path`; none, with `why` set, when it does not read, is not a
// list of maps, or a map is not one "loupe.*" key and a name.
std::optional<Recipe> load_recipe(const std::filesystem::path& path, std::string* why);
// The steps of YAML text; `path` only names it in `why`.
std::optional<std::vector<Item>> parse_recipe(const std::string& text, const std::filesystem::path& path,
                                              std::string* why);

} // namespace loupe::storyboard
