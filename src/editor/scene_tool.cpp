#include "editor/scene_tool.hpp"

#include <SDL3/SDL.h>

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

constexpr std::array<const char*, 3> FIELDS = {"position", "rotation", "scale"};

// What one tool run printed, stderr folded into stdout, and how it exited.
struct Ran {
    bool started = false;
    int exit_code = -1;
    std::string output;
};

// `args` run to the end with no shell between, through SDL_CreateProcess.
Ran
run(const std::vector<std::string>& args) {
    std::vector<const char*> argv;
    for (const std::string& arg : args) {
        argv.push_back(arg.c_str());
    }
    argv.push_back(nullptr);
    const SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, static_cast<void*>(argv.data()));
    SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDOUT_NUMBER, SDL_PROCESS_STDIO_APP);
    SDL_SetBooleanProperty(props, SDL_PROP_PROCESS_CREATE_STDERR_TO_STDOUT_BOOLEAN, true);
    SDL_Process* process = SDL_CreateProcessWithProperties(props);
    SDL_DestroyProperties(props);
    Ran ran;
    if (process == nullptr) {
        ran.output = SDL_GetError();
        return ran;
    }
    ran.started = true;
    size_t size = 0;
    void* data = SDL_ReadProcess(process, &size, &ran.exit_code);
    if (data != nullptr) {
        ran.output.assign(static_cast<const char*>(data), size);
        SDL_free(data);
    }
    SDL_DestroyProcess(process);
    return ran;
}

// autana's tool `relative` (under launcher/tools) run with `args`; false, with `why` set to what it printed, unless
// it exited 0.
bool
run_tool(const fs::path& relative, const std::vector<std::string>& args, std::string* output, std::string* why) {
    if (!builds_packs()) {
        *why = "this loupe was built without the pack tool, so it cannot run autana's tools";
        return false;
    }
    std::vector<std::string> line = {utf8(python()), utf8(autana_dir() / "launcher" / "tools" / relative)};
    line.insert(line.end(), args.begin(), args.end());
    const Ran ran = run(line);
    *output = ran.output;
    if (!ran.started || ran.exit_code != 0) {
        *why = ran.started ? ran.output : std::format("{} did not start: {}", utf8(relative), ran.output);
        while (!why->empty() && (why->back() == '\n' || why->back() == '\r')) {
            why->pop_back();
        }
        return false;
    }
    return true;
}

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

const std::array<float, 3>&
field(const Placement& placement, size_t index) {
    return index == 0 ? placement.position : index == 1 ? placement.rotation_deg : placement.scale;
}

std::array<float, 3>
read_vector(const nlohmann::json& value) {
    return {value.at(0).get<float>(), value.at(1).get<float>(), value.at(2).get<float>()};
}

} // namespace

std::optional<SceneDoc>
parse_scene(const std::string& json, const fs::path& toml, std::string* why) {
    // The tool prints its JSON as one line; anything Python said before it is not the answer.
    std::string last;
    std::istringstream lines(json);
    for (std::string line; std::getline(lines, line);) {
        if (!line.empty() && line != "\r") {
            last = line;
        }
    }
    const nlohmann::json parsed = nlohmann::json::parse(last, nullptr, false);
    if (parsed.is_discarded() || !parsed.contains("objects") || !parsed["objects"].is_array()) {
        *why = "scene_edit.py read printed no objects: " + json;
        return std::nullopt;
    }
    SceneDoc doc{.toml = toml, .objects = {}, .changed = {}};
    try {
        for (const nlohmann::json& item : parsed["objects"]) {
            Object object{.path = item.at("name").get<std::string>(),
                          .parent = item.at("parent").is_null() ? "" : item.at("parent").get<std::string>(),
                          .kind = item.at("kind").get<std::string>(),
                          .local = {.position = read_vector(item.at("position")),
                                    .rotation_deg = read_vector(item.at("rotation")),
                                    .scale = read_vector(item.at("scale"))},
                          .saved = {},
                          .baked = item.at("baked").get<bool>()};
            object.saved = object.local;
            doc.objects.push_back(std::move(object));
        }
    } catch (const nlohmann::json::exception& error) {
        *why = std::string("scene_edit.py read printed an object without its fields: ") + error.what();
        return std::nullopt;
    }
    return doc;
}

std::optional<SceneDoc>
read_scene(const fs::path& toml, std::string* why) {
    std::string output;
    if (!run_tool(fs::path("scene") / "scene_edit.py", {"read", utf8(toml)}, &output, why)) {
        return std::nullopt;
    }
    return parse_scene(output, toml, why);
}

std::vector<std::string>
save_args(const SceneDoc& doc) {
    std::vector<std::string> args;
    for (const Object& object : doc.objects) {
        for (size_t i = 0; i < FIELDS.size(); i++) {
            if (field(object.local, i) != field(object.saved, i)) {
                args.emplace_back("--set");
                args.push_back(std::format("{}.{}={}", object.path, FIELDS[i], vector_text(field(object.local, i))));
            }
        }
    }
    return args;
}

bool
save_scene(SceneDoc& doc, std::string* why) {
    std::vector<std::string> args = save_args(doc);
    if (!args.empty()) {
        args.insert(args.begin(), {"set", utf8(doc.toml)});
        std::string output;
        if (!run_tool(fs::path("scene") / "scene_edit.py", args, &output, why)) {
            return false;
        }
    }
    for (Object& object : doc.objects) {
        object.saved = object.local;
    }
    doc.changed.clear();
    return true;
}

Rebuild
rebuild(const fs::path& toml) {
    Rebuild result;
    std::string output;
    result.ok = run_tool(fs::path("bake") / "bake.py", {"list", utf8(toml), "--missing"}, &output, &result.why);
    if (result.ok) {
        // A row is tab-separated: output, kind, key, the file that asks for it. Other lines are notes.
        std::istringstream lines(output);
        for (std::string line; std::getline(lines, line);) {
            if (line.find('\t') != std::string::npos) {
                result.rebake_rows.push_back(line.substr(0, line.find('\t')));
            }
        }
    }
    return result;
}

} // namespace loupe::editor
