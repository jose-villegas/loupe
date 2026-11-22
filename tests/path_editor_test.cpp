// The editor's camera paths without a window: a key drag ends in one SetKeys,
// a path a bake reads waits for its warning, keys are added and deleted
// through the undo stack, the engine flies a camera only while its path is
// not the pack's, and the scrub bar's time maps to the frame on the shown lap.
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "editor/camera_path.hpp"
#include "window/editor.hpp"
#include "window/path_editor.hpp"

namespace {

using loupe::editor::CameraKey;

constexpr const char TEST[] = "path_editor_test";
constexpr const char CAMERA[] = "tour";
constexpr const char STREET[] = "street";
constexpr float CLOSE = 1e-4F;
// Frames this far apart, as a run at --dt 250.
constexpr uint32_t DT_MS = 250;

bool
check(bool ok, const std::string& what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what.c_str(), ok ? "ok" : "FAILED");
    return ok;
}

loupe::editor::Object
camera(const char* name, bool feeds_bakes) {
    return {.path = name,
            .parent = "",
            .kind = "camera",
            .local = {},
            .saved = {},
            .baked = false,
            .locked = "",
            .path_source =
                loupe::editor::PathSource{.keys = std::string(name) + ".keys.toml", .feeds_bakes = feeds_bakes}};
}

// Three keys over 10 s, open: the eye walks along x looking down -z.
std::vector<CameraKey>
walk() {
    std::vector<CameraKey> keys;
    for (int i = 0; i < 3; i++) {
        const auto x = static_cast<float>(i * 10);
        keys.push_back(
            {.t = static_cast<float>(i) * 5.0F, .eye = {x, 2.0F, 0.0F}, .look_at = {x, 2.0F, -10.0F}, .from = i});
    }
    return keys;
}

loupe::window::Editor
two_cameras() {
    loupe::editor::SceneDoc doc;
    doc.toml = "walk.scene.toml";
    doc.objects = {camera(CAMERA, false), camera(STREET, true)};
    loupe::window::Editor editor = loupe::window::editor_over(std::move(doc));
    for (const char* name : {CAMERA, STREET}) {
        loupe::window::open_path(editor, name,
                                 {.keys_toml = std::string(name) + ".keys.toml",
                                  .keys = walk(),
                                  .saved = walk(),
                                  .feeds_bakes = std::string(name) == STREET});
    }
    editor.selected = CAMERA;
    return editor;
}

const std::vector<CameraKey>&
keys_of(const loupe::window::Editor& editor, const char* name) {
    return editor.docs.paths.at(name).keys;
}

bool
a_drag_is_one_edit() {
    loupe::window::Editor editor = two_cameras();
    std::vector<CameraKey> moved = walk();
    moved[1].eye[1] = 3.0F;
    loupe::window::drag_keys(editor, CAMERA, moved);
    moved[1].eye[1] = 4.0F;
    loupe::window::drag_keys(editor, CAMERA, moved);
    bool ok = check(keys_of(editor, CAMERA) == walk() && loupe::window::shown_keys(editor, CAMERA) == moved
                        && !editor.edits.can_undo(),
                    "while dragged, the path shows the drag and the document waits");
    loupe::window::end_key_drag(editor);
    ok &= check(keys_of(editor, CAMERA) == moved && editor.edits.can_undo() && loupe::window::paths_unsaved(editor),
                "letting go pushes one edit");
    ok &= check(editor.edits.undo(editor.docs) && keys_of(editor, CAMERA) == walk() && !editor.edits.can_undo()
                    && !loupe::window::paths_unsaved(editor),
                "one undo takes the whole drag back");
    loupe::window::drag_keys(editor, CAMERA, moved);
    loupe::window::drag_keys(editor, CAMERA, walk());
    loupe::window::end_key_drag(editor);
    ok &= check(!editor.edits.can_undo(), "a drag back to where it began pushes nothing");
    return ok;
}

bool
a_baked_path_waits_for_its_warning() {
    loupe::window::Editor editor = two_cameras();
    bool ok = check(loupe::window::may_edit(editor, CAMERA) && !loupe::window::may_edit(editor, STREET),
                    "a path no bake reads edits at once; a baked one waits");
    editor.path.warned.insert(STREET);
    ok &= check(loupe::window::may_edit(editor, STREET), "once warned, the baked path edits");
    return ok;
}

bool
keys_are_added_and_deleted() {
    loupe::window::Editor editor = two_cameras();
    bool ok = check(loupe::window::add_key(editor, 2.5F) && keys_of(editor, CAMERA).size() == 4 && editor.path.key
                        && editor.path.key->index == 1 && std::fabs(keys_of(editor, CAMERA)[1].t - 2.5F) <= CLOSE
                        && keys_of(editor, CAMERA)[1].from == -1,
                    "a key is added on the curve and picked");
    ok &= check(!loupe::window::add_key(editor, 2.5F) && keys_of(editor, CAMERA).size() == 4,
                "a second key at the same time is refused");
    ok &= check(editor.edits.undo(editor.docs) && keys_of(editor, CAMERA) == walk(), "an add is one undo");
    std::string why;
    editor.path.key = loupe::window::KeyPick{.index = 0, .point = loupe::editor::KeyPoint::Eye, .camera = CAMERA};
    ok &= check(!loupe::window::delete_key(editor, &why) && !why.empty() && keys_of(editor, CAMERA) == walk(),
                "the first key is not deleted, and says why");
    (void)loupe::window::add_key(editor, 2.5F);
    editor.path.key = loupe::window::KeyPick{.index = 1, .point = loupe::editor::KeyPoint::Eye, .camera = CAMERA};
    ok &= check(loupe::window::delete_key(editor, &why) && keys_of(editor, CAMERA) == walk() && !editor.path.key,
                "an added key is deleted, and the pick with it");
    ok &= check(editor.edits.undo(editor.docs) && keys_of(editor, CAMERA).size() == 4, "a delete is one undo");
    return ok;
}

struct Engine {
    std::vector<std::string> flown, landed;
    loupe::Frames::PoseAt last;
    bool fails = false;
};

bool
sync(loupe::window::Editor& editor, Engine& engine) {
    engine.flown.clear();
    engine.landed.clear();
    return loupe::window::sync_paths(
        editor,
        [&engine](const std::string& name, loupe::Frames::PoseAt pose) {
            engine.flown.push_back(name);
            engine.last = std::move(pose);
            return !engine.fails;
        },
        [&engine](const std::string& name) {
            engine.landed.push_back(name);
            return !engine.fails;
        });
}

bool
the_engine_flies_only_an_edited_path() {
    loupe::window::Editor editor = two_cameras();
    Engine engine;
    bool ok = check(sync(editor, engine) && engine.flown.empty() && engine.landed.empty(),
                    "the pack's own paths: every camera stays on its clip");
    std::vector<CameraKey> moved = walk();
    moved[1].eye[1] = 6.0F;
    loupe::window::drag_keys(editor, CAMERA, moved);
    ok &= check(sync(editor, engine) && engine.flown == std::vector<std::string>{CAMERA},
                "a drag flies its camera on the dragged path");
    const loupe::PathCurve curve = loupe::editor::path_curve(moved);
    const transformf_t expected = loupe::engine_frame(loupe::pose_on(curve, 5.0F));
    const transformf_t flown = engine.last(5000);
    ok &= check(std::fabs(flown.position.y - expected.position.y) <= CLOSE
                    && std::fabs(flown.position.z - expected.position.z) <= CLOSE,
                "the camera flies the curve in the engine's frame");
    const transformf_t lapped = engine.last(10000 + 5000);
    ok &= check(std::fabs(lapped.position.y - expected.position.y) <= CLOSE, "and round again past the last key");
    ok &= check(sync(editor, engine) && engine.flown.empty(), "flown once per change");
    loupe::window::end_key_drag(editor);
    ok &= check(sync(editor, engine) && engine.flown.empty(), "the drag's end changes nothing the camera flies");
    (void)editor.edits.undo(editor.docs);
    ok &= check(sync(editor, engine) && engine.landed == std::vector<std::string>{CAMERA},
                "undone back to the pack's path, the camera lands on its clip");
    std::vector<CameraKey> faulty = walk();
    faulty[2].t = faulty[1].t;
    loupe::window::drag_keys(editor, CAMERA, faulty);
    ok &= check(sync(editor, engine) && engine.flown.empty() && engine.landed.empty(),
                "a path that would not build is not flown");
    loupe::window::drag_keys(editor, CAMERA, moved);
    engine.fails = true;
    ok &= check(!sync(editor, engine) && engine.flown.size() == 1, "a failed fly says so");
    engine.fails = false;
    ok &= check(sync(editor, engine) && engine.flown.size() == 1, "and is tried again");
    editor.path.free.on = true;
    editor.path.free_camera = CAMERA;
    loupe::window::drag_keys(editor, CAMERA, walk());
    ok &= check(sync(editor, engine) && engine.flown.empty() && engine.landed.empty(),
                "the free view's camera is left to the free view while it stands");
    editor.path.free_moved = true;
    ok &= check(sync(editor, engine) && engine.flown == std::vector<std::string>{CAMERA}
                    && std::fabs(engine.last(0).position.x - editor.path.free.fly.pose.position.x) <= CLOSE,
                "a free view move flies its camera at the free pose");
    editor.path.free.on = false;
    ok &= check(sync(editor, engine) && engine.landed == std::vector<std::string>{CAMERA} && engine.flown.empty()
                    && editor.path.free_camera.empty(),
                "the free view off, its camera lands back on the pack's path");
    return ok;
}

bool
the_scrub_seeks_on_the_shown_lap() {
    const std::vector<CameraKey> keys = walk();
    // Frame i is drawn at (i + 1) * DT_MS of play.
    bool ok = check(loupe::window::frame_at(keys, 2.0F, DT_MS, DT_MS) == 7, "2 s on the first lap is frame 7");
    ok &= check(loupe::window::frame_at(keys, 2.0F, 10500, DT_MS) == 47, "2 s on the second lap is frame 47");
    ok &= check(loupe::window::frame_at(keys, 0.0F, DT_MS, DT_MS) == loupe::Frames::FIRST_SEEKABLE,
                "the path's start seeks to the first frame a seek reaches");
    return ok;
}

} // namespace

int
main() {
    bool ok = a_drag_is_one_edit();
    ok &= a_baked_path_waits_for_its_warning();
    ok &= keys_are_added_and_deleted();
    ok &= the_engine_flies_only_an_edited_path();
    ok &= the_scrub_seeks_on_the_shown_lap();
    return ok ? 0 : 1;
}
