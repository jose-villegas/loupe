// The editor's calls into autana's tools, each a subprocess through SDL3's
// SDL_CreateProcess: scene_edit.py reads a scene's objects and saves changed
// placements (comments and layout kept), and bake.py names the bakes a saved
// scene re-keys. A build without the pack tool refuses each call.
#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "editor/doc.hpp"

namespace loupe::editor {

// The scene's objects as scene_edit.py reads them; none with `why` set.
std::optional<SceneDoc> read_scene(const std::filesystem::path& toml, std::string* why);
// The changed objects' placements written into `doc.toml`; `changed` cleared
// when it saved. False with `why` set, the file untouched.
bool save_scene(SceneDoc& doc, std::string* why);

struct Rebuild {
    bool ok = false;
    // bake.py list --missing's rows: the bakes the saved scene re-keys.
    std::vector<std::string> rebake_rows;
    std::string why;
};

// After a save: the bakes `toml` now misses.
Rebuild rebuild(const std::filesystem::path& toml);

// The scene_edit.py set arguments for `doc`'s changed objects: one
// --set NAME.FIELD=X,Y,Z per field, each number in the fewest digits that
// read back as the same float.
std::vector<std::string> save_args(const SceneDoc& doc);
// read_scene's parse of scene_edit.py read's JSON; none with `why` set.
std::optional<SceneDoc> parse_scene(const std::string& json, const std::filesystem::path& toml, std::string* why);

} // namespace loupe::editor
