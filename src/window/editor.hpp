// The scene editor's panels over an open .scene.toml: the outliner, the
// inspector and the move/rotate/scale handles over the viewport's frame. Every
// change goes through the one EditStack, and the engine follows the document,
// so what the viewport shows is what a save writes.
#pragma once

#include <filesystem>
#include <future>
#include <map>
#include <optional>
#include <string>

#include "editor/doc.hpp"
#include "editor/scene_tool.hpp"
#include "engine/frames.hpp"
#include "imgui.h"
// After imgui.h, whose types its header uses without including it.
#include "ImGuizmo.h"

namespace loupe::window {

// The handles' operation: which part of the placement a drag moves.
enum class Handle { Move, Rotate, Scale };

// While a handle or an inspector field is dragged: the object, where it
// started, and where it is now. The drag ends in one Place edit.
struct Drag {
    std::string object;
    editor::Placement before, now;
};

// The open scene's documents and undo stack, the selection, the handle in use
// (along the object's own axes or the world's), the drag in progress, what the
// engine was last told of each object (`sent`, so only what moved is placed
// again), whether the handles were held last tick (a release ends their drag),
// and the last save's outcome with the bake check it started.
struct Editor {
    editor::Docs docs;
    editor::EditStack edits;
    std::string selected;
    Handle handle = Handle::Move;
    bool local_axes = true;
    std::optional<Drag> drag;
    bool handles_held = false;
    std::map<std::string, editor::Placement> sent;
    std::string status;
    std::future<editor::Rebuild> checking;
    std::optional<editor::Rebuild> checked;
};

// The editor over `scene`; none, with `why` set, for a pack (.apak), which
// has no scene file to write, or when autana's tool cannot read it.
std::optional<Editor> open_editor(const std::filesystem::path& scene, std::string* why);

// What the menu and the keys asked of the editor this tick.
struct EditAsk {
    bool undo = false, redo = false, save = false;
};

// Ctrl+Z, Ctrl+Y or Ctrl+Shift+Z, Ctrl+S; with `letters`, W, E and R pick the
// handle when no text field has the keyboard (the observer flies on WASD).
EditAsk edit_keys(Editor& editor, bool letters);
// Applies `ask`: undo, redo, or a save through autana's tool, which then
// starts the bake check (bake.py list --missing) in the background.
void apply(Editor& editor, const EditAsk& ask);

// The scene's objects by parent; a click selects.
void draw_outliner(Editor& editor);
// The selected object's placement as numbers. Baked renderers show a locked
// badge, and lights and cameras are read-only too: E1 places renderers that
// are not baked. Below it, the last save and which bakes it re-keys.
void draw_inspector(Editor& editor);
// The handle picker and the axes toggle, for the viewport's top.
void draw_handle_bar(Editor& editor);
// The handles on the selected object over the frame drawn at `min`..`max`,
// and the view cube in its corner; a finished drag pushes one Place edit.
void draw_handles(Editor& editor, const Frames& frames, ImVec2 min, ImVec2 max);
// Places every object whose placement differs from what the engine was last
// told, the drag's in-progress placement first. False when a place failed.
bool sync(Editor& editor, Frames& frames);

} // namespace loupe::window
