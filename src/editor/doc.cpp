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
SceneDoc::unsaved() const {
    return std::ranges::any_of(objects, [](const Object& object) { return object.local != object.saved; });
}

bool
placeable(const Object& object) {
    return object.locked.empty();
}

Place::Place(std::string object, const Move& move) : object_(std::move(object)), move_(move) {}

void
Place::set(Docs& docs, const Placement& placement) const {
    if (Object* object = docs.scene.find(object_)) {
        object->local = placement;
    }
}

void
Place::apply(Docs& docs) {
    set(docs, move_.after);
}

void
Place::undo(Docs& docs) {
    set(docs, move_.before);
}

SetKeys::SetKeys(std::string camera, std::vector<CameraKey> before, std::vector<CameraKey> after)
    : camera_(std::move(camera)), before_(std::move(before)), after_(std::move(after)) {}

void
SetKeys::set(Docs& docs, const std::vector<CameraKey>& keys) const {
    if (const auto path = docs.paths.find(camera_); path != docs.paths.end()) {
        path->second.keys = keys;
    }
}

void
SetKeys::apply(Docs& docs) {
    set(docs, after_);
}

void
SetKeys::undo(Docs& docs) {
    set(docs, before_);
}

void
EditStack::push(std::unique_ptr<Edit> edit, Docs& docs) {
    edits_.resize(done_);
    edit->apply(docs);
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
    return true;
}

bool
EditStack::redo(Docs& docs) {
    if (!can_redo()) {
        return false;
    }
    edits_[done_]->apply(docs);
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

} // namespace loupe::editor
