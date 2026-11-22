#include "editor/scene_tool.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <format>
#include <functional>
#include <optional>
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

// set-keys' FROM for a key added since the file was read.
constexpr const char* NEW_KEY = "new";

// `value` as the shortest text that reads back as the same float.
std::string
number_text(float value) {
    std::array<char, 32> digits{};
    const auto [end, error] = std::to_chars(digits.data(), digits.data() + digits.size(), value);
    return {digits.data(), error == std::errc{} ? end : digits.data()};
}

// `values` as X,Y,Z, each number_text.
std::string
vector_text(const std::array<float, 3>& values) {
    std::string text;
    for (size_t i = 0; i < values.size(); i++) {
        text += (i > 0 ? "," : "") + number_text(values[i]);
    }
    return text;
}

std::array<float, 3>
read_vector(const nlohmann::json& value) {
    return {value.at(0).get<float>(), value.at(1).get<float>(), value.at(2).get<float>()};
}

// A camera's `path` in scene_edit.py read's JSON; none for another object or a camera on no path.
std::optional<PathSource>
read_source(const nlohmann::json& item) {
    if (!item.contains("path") || item.at("path").is_null()) {
        return std::nullopt;
    }
    const nlohmann::json& path = item.at("path");
    return PathSource{.keys = path.at("keys").is_null() ? fs::path() : from_utf8(path.at("keys").get<std::string>()),
                      .feeds_bakes = path.at("feeds_bakes").get<bool>()};
}

// A path key's set-keys argument: FROM:T:EX,EY,EZ:LX,LY,LZ, FROM the place in
// the file of the table with the key's id.
std::string
key_text(const CameraPath& path, const CameraKey& key) {
    const auto at = std::ranges::find(path.file_ids, key.id);
    const std::string from =
        key.id == 0 || at == path.file_ids.end() ? std::string(NEW_KEY) : std::to_string(at - path.file_ids.begin());
    return std::format("{}:{}:{}:{}", from, number_text(key.t), vector_text(key.eye), vector_text(key.look_at));
}

// The ids of `keys`, in order.
std::vector<uint32_t>
ids(const std::vector<CameraKey>& keys) {
    std::vector<uint32_t> out;
    out.reserve(keys.size());
    for (const CameraKey& key : keys) {
        out.push_back(key.id);
    }
    return out;
}

// scene_edit.py `command` on `toml`, `args` after it, logged in `folder`.
bool
scene_edit(const char* command, const fs::path& toml, std::vector<std::string> args, const fs::path& folder,
           std::string* why) {
    args.insert(args.begin(), {command, utf8(toml)});
    return autana_tool(SCENE_EDIT, args, folder / "scene_edit.log", why).has_value();
}

// The JSON scene_edit.py `command` writes for `toml`; none with `why` set.
std::optional<std::string>
scene_edit_json(const char* command, const fs::path& toml, const fs::path& folder, std::string* why) {
    const fs::path json = folder / (std::string(command) + ".json");
    if (!scene_edit(command, toml, {"-o", utf8(json)}, folder, why)) {
        return std::nullopt;
    }
    return read_text(json);
}

// Calls `each` on every item of the array `list` in scene_edit.py `command`'s
// JSON; false with `why` set when it has no such array or an item lacks a field.
bool
each_item(const std::string& json, const char* list, const char* command,
          const std::function<void(const nlohmann::json&)>& each, std::string* why) {
    const nlohmann::json parsed = nlohmann::json::parse(json, nullptr, false);
    if (parsed.is_discarded() || !parsed.contains(list) || !parsed[list].is_array()) {
        *why = std::format("scene_edit.py {} gave no {}: {}", command, list, json);
        return false;
    }
    try {
        for (const nlohmann::json& item : parsed[list]) {
            each(item);
        }
    } catch (const nlohmann::json::exception& error) {
        *why = std::format("scene_edit.py {} gave {} without their fields: {}", command, list, error.what());
        return false;
    }
    return true;
}

} // namespace

std::optional<SceneDoc>
parse_scene(const std::string& json, const fs::path& toml, std::string* why) {
    SceneDoc doc{.toml = toml, .objects = {}};
    const auto add = [&doc](const nlohmann::json& item) {
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
                      .locked = text_or_empty("locked"),
                      .path_source = read_source(item)};
        object.saved = object.local;
        doc.objects.push_back(std::move(object));
    };
    return each_item(json, "objects", "read", add, why) ? std::optional(std::move(doc)) : std::nullopt;
}

std::optional<SceneDoc>
read_scene(const fs::path& toml, const fs::path& folder, std::string* why) {
    const std::optional<std::string> json = scene_edit_json("read", toml, folder, why);
    return json ? parse_scene(*json, toml, why) : std::nullopt;
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
    if (!scene_edit("set", doc.toml, std::move(args), folder, why)) {
        return false;
    }
    for (Object& object : doc.objects) {
        object.saved = object.local;
    }
    return true;
}

std::optional<CameraPath>
parse_path(const std::string& json, const fs::path& keys_toml, std::string* why) {
    CameraPath path{.keys_toml = keys_toml, .keys = {}, .saved = {}, .file_ids = {}};
    const auto add = [&path](const nlohmann::json& item) {
        path.keys.push_back({.t = item.at("t").get<float>(),
                             .eye = read_vector(item.at("eye")),
                             .look_at = read_vector(item.at("look_at")),
                             .id = next_key_id()});
    };
    if (!each_item(json, "keys", "read-keys", add, why)) {
        return std::nullopt;
    }
    path.saved = path.keys;
    path.file_ids = ids(path.keys);
    return path;
}

std::optional<CameraPath>
read_path(const fs::path& keys_toml, const fs::path& folder, std::string* why) {
    const std::optional<std::string> json = scene_edit_json("read-keys", keys_toml, folder, why);
    return json ? parse_path(*json, keys_toml, why) : std::nullopt;
}

std::vector<std::string>
path_args(const CameraPath& path) {
    std::vector<std::string> args;
    if (!path.unsaved()) {
        return args;
    }
    for (const CameraKey& key : path.keys) {
        args.emplace_back("--key");
        args.push_back(key_text(path, key));
    }
    return args;
}

bool
save_path(CameraPath& path, const fs::path& folder, std::string* why) {
    std::vector<std::string> args = path_args(path);
    if (args.empty()) {
        return true;
    }
    if (!scene_edit("set-keys", path.keys_toml, std::move(args), folder, why)) {
        return false;
    }
    // The file now holds the keys' tables in this order; a key never in a file takes an id for its own.
    for (CameraKey& key : path.keys) {
        key.id = key.id == 0 ? next_key_id() : key.id;
    }
    path.saved = path.keys;
    path.file_ids = ids(path.keys);
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
