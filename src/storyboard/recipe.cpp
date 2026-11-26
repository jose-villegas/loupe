#include "storyboard/recipe.hpp"

#include <cstddef>
#include <format>
#include <fstream>
#include <sstream>

#include <yaml-cpp/yaml.h>

#include "paths.hpp"

namespace loupe::storyboard {

namespace {

// The key that labels a step.
constexpr const char NAME[] = "name";

// A YAML node as JSON. Scalars stay strings, as YAML gives no type the
// step's reader could trust: the reader asks for what it needs.
nlohmann::json
to_json(const YAML::Node& node) {
    switch (node.Type()) {
        case YAML::NodeType::Sequence: {
            nlohmann::json list = nlohmann::json::array();
            for (const YAML::Node& item : node) {
                list.push_back(to_json(item));
            }
            return list;
        }
        case YAML::NodeType::Map: {
            nlohmann::json map = nlohmann::json::object();
            for (const auto& entry : node) {
                map[entry.first.as<std::string>()] = to_json(entry.second);
            }
            return map;
        }
        case YAML::NodeType::Scalar: return node.Scalar();
        default: return nullptr;
    }
}

} // namespace

std::string
Item::label() const {
    if (raw.contains(NAME) && raw[NAME].is_string()) {
        return raw[NAME].get<std::string>();
    }
    return key;
}

std::optional<std::vector<Item>>
parse_recipe(const std::string& text, const std::filesystem::path& path, std::string* why) {
    const std::string name = utf8(path);
    YAML::Node root;
    try {
        root = YAML::Load(text);
    } catch (const YAML::Exception& error) {
        *why = std::format("{}:{}: {}", name, error.mark.line + 1, error.msg);
        return std::nullopt;
    }
    if (!root.IsSequence()) {
        *why = std::format("{}: a recipe is a list of steps", name);
        return std::nullopt;
    }
    std::vector<Item> items;
    for (const YAML::Node& step : root) {
        const int line = step.Mark().line + 1;
        if (!step.IsMap()) {
            *why = std::format("{}:{}: a step is a map, such as {{loupe.wait: 4}}", name, line);
            return std::nullopt;
        }
        Item item{.raw = to_json(step), .key = "", .line = line};
        size_t keys = 0;
        for (const auto& [key, value] : item.raw.items()) {
            if (key != NAME) {
                item.key = key;
                keys++;
            }
        }
        if (keys != 1 || !item.key.starts_with(LOUPE_KEY_PREFIX)) {
            *why = std::format("{}:{}: a step is one {}* key, and a name if it has one", name, line, LOUPE_KEY_PREFIX);
            return std::nullopt;
        }
        items.push_back(std::move(item));
    }
    return items;
}

std::optional<Recipe>
load_recipe(const std::filesystem::path& path, std::string* why) {
    const std::ifstream file(path, std::ios::binary);
    if (!file) {
        *why = std::format("cannot read {}", utf8(path));
        return std::nullopt;
    }
    std::ostringstream text;
    text << file.rdbuf();
    std::optional<std::vector<Item>> steps = parse_recipe(text.str(), path, why);
    if (!steps) {
        return std::nullopt;
    }
    return Recipe{.path = path, .steps = std::move(*steps)};
}

} // namespace loupe::storyboard
