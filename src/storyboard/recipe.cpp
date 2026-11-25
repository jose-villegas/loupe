#include "storyboard/recipe.hpp"

#include <cstddef>
#include <format>
#include <fstream>
#include <sstream>

#include <yaml-cpp/yaml.h>

#include "paths.hpp"

namespace loupe::storyboard {

namespace {

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

std::optional<std::string>
Item::ours() const {
    for (const auto& [key, value] : raw.items()) {
        if (key.starts_with(LOUPE_KEY_PREFIX)) {
            return key;
        }
    }
    return std::nullopt;
}

std::string
Item::label() const {
    if (raw.contains("name") && raw["name"].is_string()) {
        return raw["name"].get<std::string>();
    }
    return ours().value_or(raw.empty() ? std::string() : raw.begin().key());
}

std::optional<std::vector<Item>>
parse_recipe(const std::string& text, const std::string& path, std::string* why) {
    YAML::Node root;
    try {
        root = YAML::Load(text);
    } catch (const YAML::Exception& error) {
        *why = std::format("{}:{}: {}", path, error.mark.line + 1, error.msg);
        return std::nullopt;
    }
    if (!root.IsSequence()) {
        *why = std::format("{}: a recipe is a list of steps", path);
        return std::nullopt;
    }
    std::vector<Item> items;
    for (const YAML::Node& step : root) {
        const int line = step.Mark().line + 1;
        if (!step.IsMap()) {
            *why = std::format("{}:{}: a step is a map, such as {{loupe.wait: 4}}", path, line);
            return std::nullopt;
        }
        Item item{.raw = to_json(step), .line = line};
        size_t ours = 0;
        for (const auto& [key, value] : item.raw.items()) {
            ours += key.starts_with(LOUPE_KEY_PREFIX) ? 1 : 0;
        }
        if (ours > 1) {
            *why = std::format("{}:{}: one {}* key per step", path, line, LOUPE_KEY_PREFIX);
            return std::nullopt;
        }
        items.push_back(std::move(item));
    }
    return items;
}

std::optional<std::vector<Item>>
load_recipe(const std::filesystem::path& path, std::string* why) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        *why = std::format("cannot read {}", utf8(path));
        return std::nullopt;
    }
    std::ostringstream text;
    text << file.rdbuf();
    return parse_recipe(text.str(), utf8(path), why);
}

} // namespace loupe::storyboard
