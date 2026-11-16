// The editor's calls into autana's tools (tools.hpp's autana_tool):
// scene_edit.py reads a scene's objects and saves changed placements
// (comments and layout kept), and bake.py names the bakes a saved scene
// re-keys. Each writes its log, and the read its JSON, into `folder`. A build
// without the pack tool refuses each call.
#pragma once

#include <filesystem>
#include <optional>
#include <stop_token>
#include <string>
#include <vector>

#include "editor/doc.hpp"

namespace loupe::editor {

// The scene's objects as scene_edit.py reads them; none with `why` set.
std::optional<SceneDoc> read_scene(const std::filesystem::path& toml, const std::filesystem::path& folder,
                                   std::string* why);
// The moved fields written into `doc.toml`, then every object's `saved` set
// to its `local`. False with `why` set, the file and `doc` untouched.
bool save_scene(SceneDoc& doc, const std::filesystem::path& folder, std::string* why);

struct Rebuild {
    bool ok = false;
    // bake.py list --missing's rows: the bakes the saved scene re-keys.
    std::vector<std::string> rebake_rows;
    std::string why;
};

// After a save: the bakes `toml` now misses. A request on `stop` ends it.
Rebuild rebuild(const std::filesystem::path& toml, const std::filesystem::path& folder,
                const std::stop_token& stop = {});

// The scene_edit.py set arguments for `doc`'s moved fields: one
// --set NAME.FIELD=X,Y,Z each, each number in the fewest digits that read
// back as the same float.
std::vector<std::string> save_args(const SceneDoc& doc);
// read_scene's parse of scene_edit.py read's JSON; none with `why` set.
std::optional<SceneDoc> parse_scene(const std::string& json, const std::filesystem::path& toml, std::string* why);
// rebuild's parse of bake.py list's output: the first column of each
// tab-separated row; other lines are notes.
std::vector<std::string> rebake_rows(const std::string& listing);

} // namespace loupe::editor
