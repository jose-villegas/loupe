#include "editor/doc.hpp"

#include <algorithm>
#include <utility>

namespace loupe::editor {

Object*
SceneDoc::find(const std::string& path) {
    const auto it = std::ranges::find(objects, path, &Object::path);
    return it == objects.end() ? nullptr : &*it;
}

const Object*
SceneDoc::find(const std::string& path) const {
    const auto it = std::ranges::find(objects, path, &Object::path);
    return it == objects.end() ? nullptr : &*it;
}

bool
placeable(const Object& object) {
    return (object.kind == "mesh_renderer" || object.kind == "skinned_renderer") && !object.baked;
}

Place::Place(std::string object, Placement before, Placement after)
    : object_(std::move(object)), before_(before), after_(after) {}

void
Place::set(Docs& docs, const Placement& placement) const {
    if (Object* object = docs.scene.find(object_)) {
        object->local = placement;
        docs.scene.changed.insert(object_);
    }
}

void
Place::apply(Docs& docs) {
    set(docs, after_);
}

void
Place::undo(Docs& docs) {
    set(docs, before_);
}

bool
Place::needs_rebake() const {
    return false;
}

const std::string&
Place::object() const {
    return object_;
}

void
EditStack::push(std::unique_ptr<Edit> edit, Docs& docs) {
    edits_.resize(done_);
    edit->apply(docs);
    last_ = edit.get();
    edits_.push_back(std::move(edit));
    done_ = edits_.size();
}

bool
EditStack::undo(Docs& docs) {
    if (!can_undo()) {
        return false;
    }
    done_--;
    edits_[done_]->undo(docs);
    last_ = edits_[done_].get();
    return true;
}

bool
EditStack::redo(Docs& docs) {
    if (!can_redo()) {
        return false;
    }
    edits_[done_]->apply(docs);
    last_ = edits_[done_].get();
    done_++;
    return true;
}

bool
EditStack::can_undo() const {
    return done_ > 0;
}

bool
EditStack::can_redo() const {
    return done_ < edits_.size();
}

const Edit*
EditStack::last() const {
    return last_;
}

} // namespace loupe::editor
