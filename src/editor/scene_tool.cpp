#include "editor/scene_tool.hpp"

#include <array>
#include <charconv>
#include <cstddef>
#include <format>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

#include <nlohmann/json.hpp>

#include "paths.hpp"
#include "tools.hpp"

namespace fs = std::filesystem;

namespace loupe::editor {

namespace {

constexpr const char* SCENE_EDIT = "scene/scene_edit.py";
constexpr const char* BAKE = "bake/bake.py";

// `values` as X,Y,Z, each the shortest text that reads back as the same float.
std::string
vector_text(const std::array<float, 3>& values) {
    std::string text;
    for (size_t i = 0; i < values.size(); i++) {
        std::array<char, 32> digits{};
        const auto [end, error] = std::to_chars(digits.begin(), digits.end(), values[i]);
        text += (i > 0 ? "," : "") + std::string(digits.data(), error == std::errc{} ? end : digits.data());
    }
    return text;
}

std::array<float, 3>
read_vector(const nlohmann::json& value) {
    return {value.at(0).get<float>(), value.at(1).get<float>(), value.at(2).get<float>()};
}

} // namespace

std::optional<SceneDoc>
parse_scene(const std::string& json, const fs::path& toml, std::string* why) {
    const nlohmann::json parsed = nlohmann::json::parse(json, nullptr, false);
    if (parsed.is_discarded() || !parsed.contains("objects") || !parsed["objects"].is_array()) {
        *why = "scene_edit.py read gave no objects: " + json;
        return std::nullopt;
    }
    SceneDoc doc{.toml = toml, .objects = {}};
    try {
        for (const nlohmann::json& item : parsed["objects"]) {
            const auto text_or_empty = [&item](const char* key) {
                return item.at(key).is_null() ? std::string() : item.at(key).get<std::string>();
            };
            Object object{.path = item.at("name").get<std::string>(),
                          .parent = text_or_empty("parent"),
                          .kind = item.at("kind").get<std::string>(),
                          .local = {.position = read_vector(item.at("position")),
                                    .rotation_deg = read_vector(item.at("rotation")),
                                    .scale = read_vector(item.at("scale"))},
                          .saved = {},
                          .baked = item.at("baked").get<bool>(),
                          .locked = text_or_empty("locked")};
            object.saved = object.local;
            doc.objects.push_back(std::move(object));
        }
    } catch (const nlohmann::json::exception& error) {
        *why = std::string("scene_edit.py read gave an object without its fields: ") + error.what();
        return std::nullopt;
    }
    return doc;
}

std::optional<SceneDoc>
read_scene(const fs::path& toml, const fs::path& folder, std::string* why) {
    const fs::path json = folder / "scene.json";
    if (!autana_tool(SCENE_EDIT, {"read", utf8(toml), "-o", utf8(json)}, folder / "scene_edit.log", why)) {
        return std::nullopt;
    }
    return parse_scene(read_text(json), toml, why);
}

std::vector<std::string>
save_args(const SceneDoc& doc) {
    std::vector<std::string> args;
    for (const Object& object : doc.objects) {
        for (const PlacementField& field : PLACEMENT_FIELDS) {
            if (object.local.*field.values != object.saved.*field.values) {
                args.emplace_back("--set");
                args.push_back(
                    std::format("{}.{}={}", object.path, field.name, vector_text(object.local.*field.values)));
            }
        }
    }
    return args;
}

bool
save_scene(SceneDoc& doc, const fs::path& folder, std::string* why) {
    std::vector<std::string> args = save_args(doc);
    if (args.empty()) {
        return true;
    }
    args.insert(args.begin(), {"set", utf8(doc.toml)});
    if (!autana_tool(SCENE_EDIT, args, folder / "scene_edit.log", why)) {
        return false;
    }
    for (Object& object : doc.objects) {
        object.saved = object.local;
    }
    return true;
}

std::vector<std::string>
rebake_rows(const std::string& listing) {
    // A row is tab-separated: output, kind, key, the file that asks for it.
    std::vector<std::string> rows;
    std::istringstream lines(listing);
    for (std::string line; std::getline(lines, line);) {
        if (const size_t tab = line.find('\t'); tab != std::string::npos) {
            rows.push_back(line.substr(0, tab));
        }
    }
    return rows;
}

Rebuild
rebuild(const fs::path& toml, const fs::path& folder, const std::stop_token& stop) {
    Rebuild result;
    const std::optional<std::string> listing =
        autana_tool(BAKE, {"list", utf8(toml), "--missing"}, folder / "bake_list.log", &result.why, stop);
    result.ok = listing.has_value();
    if (listing) {
        result.rebake_rows = rebake_rows(*listing);
    }
    return result;
}

} // namespace loupe::editor
