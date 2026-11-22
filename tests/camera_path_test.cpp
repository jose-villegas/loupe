// Camera paths without a window or a scene: the curve built as autana's
// camera_keys.py builds it (its numbers, from camera_keys.build, written here),
// what the editor refuses, the key edits, SetKeys on the one undo stack, and
// scene_edit.py's path arguments and parses.
#include <cmath>
#include <cstdio>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "checks.hpp"
#include "editor/camera_path.hpp"
#include "editor/doc.hpp"
#include "editor/scene_tool.hpp"

namespace {

using loupe::editor::CameraKey;

constexpr const char TEST[] = "camera_path_test";
// The fixed paths' key ids: far past any next_key_id a test draws.
constexpr uint32_t FIXED_ID = 1000;
constexpr float CLOSE = 1e-5F;
// A sampled pose against a key's: the curve's float maths at a key time.
constexpr float CLOSE_POSE = 1e-3F;

constexpr loupe::test::Checks check{TEST};

bool
same_numbers(const std::vector<float>& got, const std::vector<float>& want, float tolerance = CLOSE) {
    if (got.size() != want.size()) {
        return false;
    }
    for (size_t i = 0; i < got.size(); i++) {
        if (std::fabs(got[i] - want[i]) > tolerance * std::fmax(1.0F, std::fabs(want[i]))) {
            return false;
        }
    }
    return true;
}

CameraKey
key(float t, loupe::editor::Point eye, loupe::editor::Point look_at, uint32_t id) {
    return {.t = t, .eye = eye, .look_at = look_at, .id = id};
}

// Three keys, open: the eye swings round a point and rises.
std::vector<CameraKey>
open_path() {
    return {key(0.0F, {0, 1, 5}, {0, 1, 0}, FIXED_ID + 1), key(2.5F, {5, 2, 0}, {0, 1, 0}, FIXED_ID + 2),
            key(5.0F, {0, 1, -5}, {1, 3, 0}, FIXED_ID + 3)};
}

// The start of Sponza's tour, closed back on its first key.
std::vector<CameraKey>
loop_path() {
    return {key(0.0F, {600, 160, -30}, {600, 300, -330}, FIXED_ID + 1),
            key(5.0F, {300, 180, -190}, {280, 260, -340}, FIXED_ID + 2),
            key(10.0F, {-200, 230, 20}, {-200, 450, 310}, FIXED_ID + 3),
            key(15.0F, {600, 160, -30}, {600, 300, -330}, FIXED_ID + 4)};
}

bool
curve_is_camera_keys() {
    // tools/anim/camera_keys.py's build of the same keys, read back from its glTF.
    const loupe::PathCurve open = loupe::editor::path_curve(open_path());
    bool ok = check(same_numbers(open.times, {0.0F, 2.5F, 5.0F}), "the curve's times are the keys'");
    ok &= check(same_numbers(open.translations, {2, 0.4F, -2, 0,  1,  5,     2,  0.4F, -2, 0,  0,  -2,    5, 2,
                                                 0, 0,    0,  -2, -2, -0.4F, -2, 0,    1,  -5, -2, -0.4F, -2}),
                "an open path's eye tangents are camera_keys.py's");
    ok &= check(same_numbers(open.rotations, {0, 0, 0, 1, -0.0696766153F, 0.703665495F, 0.0696766153F, 0.703665495F,
                                              -0.0183092691F, 0.977803767F, -0.184905663F, -0.0968216583F}),
                "an open path's rotations are camera_keys.py's");
    const loupe::PathCurve loop = loupe::editor::path_curve(loop_path());
    ok &= check(same_numbers(loop.translations,
                             {50, -5, -21, 600,  160, -30, 50, -5, -21, -80, 7,  5,   300, 180, -190, -80, 7,  5,
                              30, -2, 16,  -200, 230, 20,  30, -2, 16,  50,  -5, -21, 600, 160, -30,  50,  -5, -21}),
                "a loop's eye tangents close through the join, as camera_keys.py's");
    ok &= check(same_numbers(loop.rotations,
                             {0.216583535F, 0, 0, 0.97626406F, 0.240236387F, 0.0642790794F, -0.0159452036F,
                              0.968452573F, 0, 0.947810948F,
                              -0.318832815F, // NOLINT(modernize-use-std-numbers): camera_keys.py's number, not 1/pi
                              0, 0.216583535F, 0, 0, 0.97626406F}),
                "a loop's rotations stay on one side, as camera_keys.py's");
    ok &= check(loupe::editor::loops(loop_path()) && !loupe::editor::loops(open_path()),
                "a path whose last key repeats its first loops");
    return ok;
}

bool
faults_are_camera_keys() {
    using loupe::editor::path_fault;
    std::vector<CameraKey> keys = open_path();
    bool ok = check(!path_fault(keys), "a good path has no fault");
    ok &= check(path_fault({keys[0]}).has_value(), "one key is a fault");
    keys[0].t = 1.0F;
    ok &= check(path_fault(keys).has_value(), "a path that does not start at zero is a fault");
    keys = open_path();
    keys[2].t = keys[1].t;
    ok &= check(path_fault(keys).has_value(), "two keys at one time are a fault");
    keys = open_path();
    keys[1].look_at = keys[1].eye;
    ok &= check(path_fault(keys).has_value(), "a key looking at its own eye is a fault");
    keys = open_path();
    keys[1].look_at = {keys[1].eye[0], keys[1].eye[1] + 1.0F, keys[1].eye[2]};
    ok &= check(path_fault(keys).has_value(), "a key looking straight up is a fault");
    keys = open_path();
    keys[1].eye[0] = std::nanf("");
    ok &= check(path_fault(keys).has_value(), "a number that is not finite is a fault");
    keys = open_path();
    while (keys.size() <= loupe::MAX_CURVE_KEYS) {
        CameraKey next = keys.back();
        next.t += 1.0F;
        keys.push_back(next);
    }
    ok &= check(path_fault(keys).has_value(), "more keys than a clip's track counts is a fault");
    keys.pop_back();
    ok &= check(!path_fault(keys), "as many keys as a clip's track counts are a path");
    return ok;
}

bool
poses_lie_on_the_keys() {
    const std::vector<CameraKey> keys = open_path();
    const loupe::PathCurve curve = loupe::editor::path_curve(keys);
    bool ok = true;
    for (const CameraKey& at : keys) {
        const CameraKey on = loupe::editor::key_on(curve, keys, at.t);
        bool same = on.id == 0;
        for (size_t i = 0; i < 3; i++) {
            same &= std::fabs(on.eye[i] - at.eye[i]) <= CLOSE_POSE
                    && std::fabs(on.look_at[i] - at.look_at[i]) <= CLOSE_POSE;
        }
        ok &= check(same, "a key made on the curve at key time " + std::to_string(at.t) + " is that key");
    }
    ok &= check(loupe::editor::path_end(keys) == 5.0F, "the path ends at its last key");
    return ok;
}

bool
clip_time_laps_at_the_last_key() {
    using loupe::editor::clip_seconds;
    using loupe::editor::lap_start_ms;
    std::vector<CameraKey> keys = open_path(); // ends at 5 s
    bool ok =
        check(lap_start_ms(keys, 4999) == 0 && lap_start_ms(keys, 5000) == 5000 && lap_start_ms(keys, 12500) == 10000,
              "a lap begins at each whole path's length of play");
    ok &= check(clip_seconds(keys, 5000) == 0.0F && clip_seconds(keys, 12500) == 2.5F,
                "on a lap's boundary the clip is at its first key, and the third lap runs as the first");
    keys.back().t = 4.9996F;
    ok &= check(lap_start_ms(keys, 5000) == 5000, "a path's length rounds to the nearest millisecond");
    ok &=
        check(lap_start_ms({}, 1234) == 0 && clip_seconds({}, 1234) == 0.0F, "a path of no length stays at its start");
    return ok;
}

bool
key_edits_keep_the_path_whole() {
    using loupe::editor::clamp_time;
    std::vector<CameraKey> loop = loop_path();
    CameraKey moved = loop[0];
    moved.eye = {610, 160, -30};
    std::vector<CameraKey> edited = loupe::editor::with_key(loop, 0, moved);
    bool ok = check(edited[0].eye == moved.eye && edited[3].eye == moved.eye && edited[3].t == 15.0F,
                    "on a loop, the first key's move moves the last too, its time kept");
    ok &= check(edited[3].id == loop[3].id && edited[0].id == loop[0].id, "an edit keeps each key's id");
    moved = loop[3];
    moved.look_at = {600, 310, -330};
    edited = loupe::editor::with_key(loop, 3, moved);
    ok &= check(edited[0].look_at == moved.look_at, "on a loop, the last key's move moves the first too");
    std::vector<CameraKey> open = open_path();
    moved = open[0];
    moved.eye = {1, 1, 5};
    ok &= check(loupe::editor::with_key(open, 0, moved)[2].eye == open[2].eye, "an open path ties nothing");

    ok &= check(clamp_time(open, 0, 3.0F) == 0.0F, "the first key stays at zero");
    ok &= check(clamp_time(open, 1, 9.0F) == 5.0F - loupe::editor::TIME_STEP_S, "a key stays before the next");
    ok &= check(clamp_time(open, 1, -1.0F) == loupe::editor::TIME_STEP_S, "a key stays after the one before");
    ok &= check(clamp_time(open, 2, 9.0F) == 9.0F, "the last key may move later");
    ok &= check(std::fabs(clamp_time(open, 1, 1.23456F) - 1.235F) < CLOSE, "a time rounds to milliseconds");

    size_t added = 0;
    const std::optional<std::vector<CameraKey>> inside = loupe::editor::with_key_added(open, 1.0F, &added);
    ok &= check(inside && inside->size() == 4 && added == 1 && (*inside)[1].t == 1.0F && (*inside)[1].id < FIXED_ID
                    && (*inside)[2].id == open[1].id && !loupe::editor::path_fault(*inside),
                "a key added between two goes between them, with an id of its own, and the path still builds");
    const std::optional<std::vector<CameraKey>> after = loupe::editor::with_key_added(open, 5.0F, &added);
    ok &= check(after && added == 3 && (*after)[3].t == 7.5F && (*after)[3].eye == open[2].eye,
                "a key added at the end goes one spacing past it, at the last key");
    ok &= check(!loupe::editor::with_key_added(open, 2.5F + loupe::editor::TIME_STEP_S / 4, &added),
                "no key is added on top of another");

    ok &= check(loupe::editor::delete_refusal(open, 0).has_value(), "the first key is not deleted");
    ok &= check(loupe::editor::delete_refusal({open[0], open[1]}, 1).has_value(), "the last two keys are not deleted");
    ok &= check(!loupe::editor::delete_refusal(open, 1), "a middle key is deleted");
    return ok;
}

bool
set_keys_is_one_undo() {
    loupe::editor::Docs docs;
    docs.paths["tour"] = {.keys_toml = "tour.keys.toml",
                          .keys = open_path(),
                          .saved = open_path(),
                          .file_ids = {FIXED_ID + 1, FIXED_ID + 2, FIXED_ID + 3}};
    loupe::editor::EditStack edits;
    std::vector<CameraKey> after = open_path();
    after.erase(after.begin() + 1);
    edits.push(std::make_unique<loupe::editor::SetKeys>("tour", open_path(), after), docs);
    bool ok = check(docs.paths["tour"].keys == after && docs.paths["tour"].unsaved(), "SetKeys sets the keys");
    ok &= check(edits.undo(docs) && docs.paths["tour"].keys == open_path() && !docs.paths["tour"].unsaved(),
                "undoing it gives the keys back");
    ok &= check(edits.redo(docs) && docs.paths["tour"].keys == after, "redoing it sets them again");
    return ok;
}

bool
tool_calls_are_scene_edits() {
    loupe::editor::CameraPath path{.keys_toml = "walk.keys.toml",
                                   .keys = open_path(),
                                   .saved = open_path(),
                                   .file_ids = {FIXED_ID + 1, FIXED_ID + 2, FIXED_ID + 3}};
    bool ok = check(loupe::editor::path_args(path).empty(), "an unchanged path saves nothing");
    path.keys[1].eye = {5.5F, 2, 0};
    path.keys.insert(path.keys.begin() + 1, key(1.25F, {3, 1, 3}, {0, 1, 0}, 0));
    path.keys.pop_back();
    const std::vector<std::string> want = {"--key", "0:0:0,1,5:0,1,0",    "--key", "new:1.25:3,1,3:0,1,0",
                                           "--key", "1:2.5:5.5,2,0:0,1,0"};
    ok &= check(loupe::editor::path_args(path) == want, "set-keys gets the whole list, each kept key by its place");

    std::string why;
    const std::optional<loupe::editor::CameraPath> read = loupe::editor::parse_path(
        R"({"node": "camera", "animation": "walk", "keys": [{"t": 0, "eye": [0, 1, 5], "look_at": [0, 1, 0]},)"
        R"( {"t": 2.5, "eye": [5, 2, 0], "look_at": [0, 1, 0]}]})",
        "walk.keys.toml", &why);
    ok &= check(read && read->keys.size() == 2 && read->keys[0].id != 0 && read->keys[1].id != read->keys[0].id
                    && read->keys[1].t == 2.5F && read->keys == read->saved
                    && read->file_ids == std::vector<uint32_t>{read->keys[0].id, read->keys[1].id},
                "read-keys' JSON is the path, each key with an id of its own, the file's tables in order");
    ok &= check(!loupe::editor::parse_path(R"({"keys": [{"t": 0}]})", "walk.keys.toml", &why) && !why.empty(),
                "a key without its points is refused, saying why");

    const std::optional<loupe::editor::SceneDoc> scene = loupe::editor::parse_scene(
        R"({"objects": [{"name": "tour", "parent": null, "kind": "camera", "baked": false, "locked": "a camera",)"
        R"( "position": [0, 0, 0], "rotation": [0, 0, 0], "scale": [1, 1, 1],)"
        R"( "path": {"anim": "a/tour.anim.toml", "keys": "a/tour.keys.toml", "feeds_bakes": true}},)"
        R"( {"name": "fly", "parent": null, "kind": "camera", "baked": false, "locked": "a camera",)"
        R"( "position": [0, 0, 0], "rotation": [0, 0, 0], "scale": [1, 1, 1],)"
        R"( "path": {"anim": "a/fly.anim.toml", "keys": null, "feeds_bakes": false}},)"
        R"( {"name": "still", "parent": null, "kind": "camera", "baked": false, "locked": "a camera",)"
        R"( "position": [0, 0, 0], "rotation": [0, 0, 0], "scale": [1, 1, 1], "path": null}]})",
        "a/s.scene.toml", &why);
    if (!scene || scene->objects.size() != 3) {
        return check(false, "the scene's JSON is read: " + why);
    }
    const std::vector<loupe::editor::Object>& objects = scene->objects;
    const std::optional<loupe::editor::PathSource>& tour = objects[0].path_source;
    const std::optional<loupe::editor::PathSource>& fly = objects[1].path_source;
    ok &= check(tour && tour->keys == "a/tour.keys.toml" && tour->feeds_bakes,
                "a camera's keys path and whether bakes read it are read");
    ok &= check(fly && fly->keys.empty(), "a camera on a .glb clip has a path with no keys file");
    ok &= check(!objects[2].path_source, "a camera on no path has none");
    return ok;
}

} // namespace

int
main() {
    bool ok = curve_is_camera_keys();
    ok &= faults_are_camera_keys();
    ok &= poses_lie_on_the_keys();
    ok &= clip_time_laps_at_the_last_key();
    ok &= key_edits_keep_the_path_whole();
    ok &= set_keys_is_one_undo();
    ok &= tool_calls_are_scene_edits();
    return ok ? 0 : 1;
}
