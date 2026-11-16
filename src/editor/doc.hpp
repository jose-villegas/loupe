// What the editor holds and saves: the scene file's objects, each placed as
// the file authors it. Engine state is never saved. Every change is an Edit
// on one EditStack, so undo and redo cross every panel.
#pragma once

#include <array>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <set>
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

// One scene object. `kind` is its one component: mesh_renderer,
// skinned_renderer, light or camera. A baked renderer (bake = true) is traced
// where it sits, so it is locked. `saved` is the placement the file holds, so
// a save writes only the fields that moved.
struct Object {
    std::string path, parent;
    std::string kind;
    Placement local, saved;
    bool baked = false;
};

// The scene file and its objects in the file's order. `changed` names the
// objects edited since the last save.
struct SceneDoc {
    std::filesystem::path toml;
    std::vector<Object> objects;
    std::set<std::string> changed;

    Object* find(const std::string& path);
    const Object* find(const std::string& path) const;
};

// Whether the editor may move `object`: a renderer that is not baked. Lights
// and cameras move what every bake reads, so they stay as authored here.
bool placeable(const Object& object);

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
    // True when the edit changes what a bake reads, so the board needs a rebake to show it.
    virtual bool needs_rebake() const = 0;
};

// Moves one object from `before` to `after`. Only placeable objects are
// placed, and none of those feeds a bake.
class Place final : public Edit {
  public:
    Place(std::string object, Placement before, Placement after);
    void apply(Docs& docs) override;
    void undo(Docs& docs) override;
    bool needs_rebake() const override;
    const std::string& object() const;

  private:
    void set(Docs& docs, const Placement& placement) const;
    std::string object_;
    Placement before_, after_;
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
    // The edit the last push, undo or redo touched; null before any.
    const Edit* last() const;

  private:
    std::vector<std::unique_ptr<Edit>> edits_;
    size_t done_ = 0; // edits_[0, done_) are applied
    const Edit* last_ = nullptr;
};

} // namespace loupe::editor
