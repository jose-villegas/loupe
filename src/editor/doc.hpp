// What the editor holds and saves: the scene file's objects, each placed as
// the file authors it. Engine state is never saved. Every change is an Edit
// on one EditStack, so undo and redo cross every panel.
#pragma once

#include <array>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace loupe::editor {

// A placement as the .scene.toml writes it: position, rotation as Euler
// degrees [pitch, yaw, roll] in autana's convention (editor/placement.hpp),
// and scale.
struct Placement {
    std::array<float, 3> position{0.0F, 0.0F, 0.0F};
    std::array<float, 3> rotation_deg{0.0F, 0.0F, 0.0F};
    std::array<float, 3> scale{1.0F, 1.0F, 1.0F};
    bool operator==(const Placement&) const = default;
};

// A placement's three fields by the names the .scene.toml gives them, in the
// file's order.
struct PlacementField {
    const char* name;
    std::array<float, 3> Placement::* values;
};

constexpr std::array<PlacementField, 3> PLACEMENT_FIELDS = {{
    {.name = "position", .values = &Placement::position},
    {.name = "rotation", .values = &Placement::rotation_deg},
    {.name = "scale", .values = &Placement::scale},
}};

// One scene object. `kind` is its one component: mesh_renderer,
// skinned_renderer, light or camera; `baked` a renderer with bake = true.
// `locked` is why it may not be placed, as autana's scene_edit.py says,
// empty when it may. `saved` is the placement the file holds, so a save
// writes only the fields that moved.
struct Object {
    std::string path, parent;
    std::string kind;
    Placement local, saved;
    bool baked = false;
    std::string locked;
};

// Whether the editor may move `object`: scene_edit.py gave no reason it may not.
bool placeable(const Object& object);

// The scene file and its objects in the file's order.
struct SceneDoc {
    std::filesystem::path toml;
    std::vector<Object> objects;

    Object* find(const std::string& path);
    const Object* find(const std::string& path) const;
    // Whether any object is placed other than as the file holds it.
    bool unsaved() const;
};

// Every document the editor has open.
struct Docs {
    SceneDoc scene;
};

struct Edit {
    Edit() = default;
    Edit(const Edit&) = delete;
    Edit& operator=(const Edit&) = delete;
    Edit(Edit&&) = delete;
    Edit& operator=(Edit&&) = delete;
    virtual ~Edit() = default;
    virtual void apply(Docs& docs) = 0;
    virtual void undo(Docs& docs) = 0;
};

// Where a Place takes an object from and to.
struct Move {
    Placement before, after;
};

// Moves one object from `move.before` to `move.after`.
class Place final : public Edit {
  public:
    Place(std::string object, const Move& move);
    void apply(Docs& docs) override;
    void undo(Docs& docs) override;

  private:
    void set(Docs& docs, const Placement& placement) const;
    std::string object_;
    Move move_;
};

// The one undo history. A push applies the edit and drops whatever was
// undone past it.
class EditStack {
  public:
    void push(std::unique_ptr<Edit> edit, Docs& docs);
    // False when there is nothing to undo or redo.
    bool undo(Docs& docs);
    bool redo(Docs& docs);
    bool can_undo() const;
    bool can_redo() const;

  private:
    std::vector<std::unique_ptr<Edit>> edits_;
    size_t done_ = 0; // edits_[0, done_) are applied
};

} // namespace loupe::editor
