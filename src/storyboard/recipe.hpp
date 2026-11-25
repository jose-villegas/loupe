// A recipe: the YAML list of steps autana's runner and loupe-storyboard share.
// Each step is a map; the steps whose key starts "loupe." are Loupe's, the
// rest autana's.
#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace loupe::storyboard {

// The prefix of Loupe's step keys.
constexpr const char LOUPE_KEY_PREFIX[] = "loupe.";

// One step: its map as JSON, and the recipe line it starts on (1-based).
struct Item {
    nlohmann::json raw;
    int line = 0;

    // The step's "loupe.*" key; none for a step that is autana's.
    std::optional<std::string> ours() const;
    // The step's label: its "name", or its key.
    std::string label() const;
};

// The steps of the recipe at `path`; none, with `why` set, when it does not
// read, is not a list of maps, or a map holds two "loupe.*" keys.
std::optional<std::vector<Item>> load_recipe(const std::filesystem::path& path, std::string* why);
// The same, from YAML text; `path` only names it in `why`.
std::optional<std::vector<Item>> parse_recipe(const std::string& text, const std::string& path, std::string* why);

} // namespace loupe::storyboard
