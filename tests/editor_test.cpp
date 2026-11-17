// The editor's documents without a window: one undo stack across edits, what
// may be placed, placements to and from autana's matrices, the handles'
// matrices against autana's pinhole, the save's arguments, the read's and the
// bake list's parses, the outliner's tree, and sync's placing.
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "editor/doc.hpp"
#include "editor/placement.hpp"
#include "editor/scene_tool.hpp"
#include "window/editor.hpp"

namespace {

using loupe::editor::Placement;

constexpr const char TEST[] = "editor_test";
constexpr float CLOSE = 1e-4F;
constexpr float CLOSE_DEGREES = 1e-2F;

bool
check(bool ok, const std::string& what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what.c_str(), ok ? "ok" : "FAILED");
    return ok;
}

bool
near(float a, float b, float tolerance = CLOSE) {
    return std::fabs(a - b) <= tolerance;
}

loupe::editor::Object
object(const char* path, const char* parent, const char* locked) {
    return {.path = path,
            .parent = parent,
            .kind = "mesh_renderer",
            .local = {},
            .saved = {},
            .baked = false,
            .locked = locked};
}

loupe::editor::Docs
two_objects() {
    loupe::editor::Docs docs;
    docs.scene.objects = {object("capybara", "", ""), object("meadow", "", "'meadow' is baked")};
    return docs;
}

Placement
at(float x) {
    Placement placement;
    placement.position = {x, 0.0F, 0.0F};
    return placement;
}

std::unique_ptr<loupe::editor::Place>
place(const char* name, const Placement& before, const Placement& after) {
    return std::make_unique<loupe::editor::Place>(name, loupe::editor::Move{.before = before, .after = after});
}

bool
undo_is_one_stack() {
    loupe::editor::Docs docs = two_objects();
    loupe::editor::EditStack edits;
    bool ok = check(!edits.can_undo() && !edits.can_redo() && !edits.undo(docs), "an empty stack undoes nothing");
    edits.push(place("capybara", at(0), at(1)), docs);
    edits.push(place("capybara", at(1), at(2)), docs);
    ok &= check(docs.scene.find("capybara")->local == at(2) && docs.scene.unsaved(), "a push applies its edit");
    ok &= check(edits.undo(docs) && docs.scene.find("capybara")->local == at(1), "undo goes back one edit");
    ok &= check(edits.undo(docs) && docs.scene.find("capybara")->local == at(0) && !edits.can_undo(),
                "undo goes back to the start");
    ok &= check(!docs.scene.unsaved(), "undone back to the file's placement, nothing is unsaved");
    ok &= check(edits.redo(docs) && docs.scene.find("capybara")->local == at(1), "redo comes forward one edit");
    edits.push(place("capybara", at(1), at(5)), docs);
    ok &= check(!edits.can_redo() && docs.scene.find("capybara")->local == at(5),
                "a push after an undo drops what was undone");
    edits.push(place("no_such_object", at(0), at(1)), docs);
    ok &= check(docs.scene.find("capybara")->local == at(5) && edits.undo(docs),
                "a place on a missing object changes nothing and still undoes");
    return ok;
}

bool
what_autana_locks_stays() {
    const loupe::editor::Docs docs = two_objects();
    return check(loupe::editor::placeable(*docs.scene.find("capybara"))
                     && !loupe::editor::placeable(*docs.scene.find("meadow")),
                 "an object scene_edit.py gives a reason for is not placed; one it gives none for is");
}

bool
same_matrix(const mat4f_t& a, const mat4f_t& b) {
    bool same = true;
    for (int row = 0; row < 4; row++) {
        for (int column = 0; column < 4; column++) {
            same &= near(a.m[row][column], b.m[row][column]);
        }
    }
    return same;
}

bool
placement_round_trips() {
    bool ok = true;
    const std::vector<Placement> placements = {
        {.position = {1.5F, -2.0F, 0.25F}, .rotation_deg = {40.0F, 30.0F, 0.0F}, .scale = {1.0F, 1.0F, 1.0F}},
        {.position = {0.0F, 0.0F, 0.0F}, .rotation_deg = {-12.2F, 43.8F, 17.0F}, .scale = {1.5F, 2.0F, 0.5F}},
        {.position = {3.0F, 4.0F, 5.0F}, .rotation_deg = {80.0F, -170.0F, 95.0F}, .scale = {2.0F, 2.0F, 2.0F}},
    };
    for (const Placement& placement : placements) {
        const std::optional<Placement> back = loupe::editor::to_placement(loupe::editor::to_matrix(placement));
        bool same = back.has_value();
        for (size_t i = 0; back && same && i < 3; i++) {
            same &= near(back->position[i], placement.position[i])
                    && near(back->rotation_deg[i], placement.rotation_deg[i], CLOSE_DEGREES)
                    && near(back->scale[i], placement.scale[i]);
        }
        ok &= check(same, "a placement comes back from its matrix");
    }
    // At pitch 90 yaw and roll turn about one axis: the angles differ, the matrix does not.
    const Placement locked = {.position = {}, .rotation_deg = {90.0F, 20.0F, 30.0F}, .scale = {1.0F, 1.0F, 1.0F}};
    const std::optional<Placement> back = loupe::editor::to_placement(loupe::editor::to_matrix(locked));
    ok &= check(back && same_matrix(loupe::editor::to_matrix(*back), loupe::editor::to_matrix(locked)),
                "at pitch 90 the placement read back is the same turn");
    // What a placement cannot hold: shear, a mirror, a zero scale.
    mat4f_t sheared = loupe::editor::to_matrix(placements[0]);
    sheared.m[0][1] += 0.5F;
    Placement mirrored = placements[1], flat = placements[1];
    mirrored.scale[0] = -1.0F;
    flat.scale[2] = 0.0F;
    ok &=
        check(!loupe::editor::to_placement(sheared) && !loupe::editor::to_placement(loupe::editor::to_matrix(mirrored))
                  && !loupe::editor::to_placement(loupe::editor::to_matrix(flat)),
              "a sheared, mirrored or flattened matrix has no placement");
    return ok;
}

bool
matrices() {
    const mat4f_t matrix = loupe::editor::to_matrix(
        {.position = {1.0F, 2.0F, 3.0F}, .rotation_deg = {10.0F, 20.0F, 30.0F}, .scale = {2.0F, 1.0F, 0.5F}});
    bool ok = check(same_matrix(loupe::editor::mirror_z(loupe::editor::mirror_z(matrix)), matrix),
                    "mirror_z twice is no mirror");
    // ImGuizmo reads a column-major float[16]: the translation is elements 12..14.
    const loupe::editor::GizmoMatrix columns = loupe::editor::gizmo(matrix);
    ok &= check(columns[12] == 1.0F && columns[13] == 2.0F && columns[14] == 3.0F && columns[1] == matrix.m[1][0]
                    && same_matrix(loupe::editor::from_gizmo(columns), matrix),
                "the handles' matrix is column-major and comes back as it was");
    // A point the engine's pinhole puts at pixel (px, py) lands there through the handles' matrices.
    const loupe::ViewCamera camera = {.eye = {1.0F, 2.0F, -5.0F},
                                      .right = vec3f_unit_x(),
                                      .up = vec3f_unit_y(),
                                      .forward = vec3f_unit_z(),
                                      .fx = 300.0F,
                                      .fy = 300.0F,
                                      .cx = 190.0F,
                                      .cy = 220.0F,
                                      .near_z = 0.05F,
                                      .width = 368,
                                      .height = 448};
    const vec3f_t point = {1.6F, 2.9F, 1.0F};
    const vec3f_t offset = vec3f_sub(point, camera.eye);
    const float px = camera.cx + camera.fx * vec3f_dot(offset, camera.right) / vec3f_dot(offset, camera.forward);
    const float py = camera.cy - camera.fy * vec3f_dot(offset, camera.up) / vec3f_dot(offset, camera.forward);
    const mat4f_t world = mat4f_from_trs(point, quatf_identity(), vec3f_one());
    const mat4f_t clip = mat4f_mul(loupe::editor::projection_matrix(camera, 1000.0F),
                                   mat4f_mul(loupe::editor::view_matrix(camera), loupe::editor::mirror_z(world)));
    const float w = clip.m[3][3];
    ok &= check(w > 0 && near(clip.m[0][3] / w, 2.0F * px / 368.0F - 1.0F)
                    && near(clip.m[1][3] / w, 1.0F - 2.0F * py / 448.0F),
                "the handles' view and projection put a point where autana's pinhole does");
    return ok;
}

bool
save_writes_only_what_moved() {
    loupe::editor::SceneDoc doc = two_objects().scene;
    doc.toml = "capybara.scene.toml";
    loupe::editor::Object& capybara = *doc.find("capybara");
    capybara.local.position = {0.5F, 0.0F, -0.25F};
    capybara.local.scale = {1.5F, 1.5F, 1.5F};
    doc.find("meadow")->local.rotation_deg = {0.1F, -90.0F, 1e-7F};
    const std::vector<std::string> expected = {"--set", "capybara.position=0.5,0,-0.25",
                                               "--set", "capybara.scale=1.5,1.5,1.5",
                                               "--set", "meadow.rotation=0.1,-90,1e-07"};
    bool ok = check(loupe::editor::save_args(doc) == expected,
                    "the save sets only the fields that moved, every object's, in the fewest digits");
    for (loupe::editor::Object& item : doc.objects) {
        item.saved = item.local;
    }
    ok &= check(loupe::editor::save_args(doc).empty(), "nothing moved since the save: nothing to set");
    return ok;
}

bool
reads_parse_the_tools_output() {
    std::string why;
    const std::string json =
        "{\"objects\": [{\"name\": \"lid\", \"parent\": \"box\", \"kind\": \"mesh_renderer\", \"baked\": false, "
        "\"locked\": null, \"position\": [0.0, 1.0, 0.0], \"rotation\": [0.0, 90.0, 0.0], \"scale\": [1.0, 1.0, 1.0]}, "
        "{\"name\": \"sun\", \"parent\": null, \"kind\": \"light\", \"baked\": false, \"locked\": \"'sun' is a "
        "light\", "
        "\"position\": [0.0, 0.0, 0.0], \"rotation\": [40.0, 30.0, 0.0], \"scale\": [1.0, 1.0, 1.0]}]}\n";
    const std::optional<loupe::editor::SceneDoc> doc = loupe::editor::parse_scene(json, "x.scene.toml", &why);
    bool ok = check(doc && doc->objects.size() == 2 && doc->objects[0].parent == "box"
                        && doc->objects[0].local.rotation_deg[1] == 90.0F
                        && doc->objects[0].saved == doc->objects[0].local && doc->objects[0].locked.empty()
                        && doc->objects[1].locked == "'sun' is a light" && doc->objects[1].parent.empty(),
                    "the read's JSON becomes the document, saved as read, with autana's lock reasons");
    ok &= check(!loupe::editor::parse_scene("scene_edit: refused\n", "x.scene.toml", &why) && !why.empty(),
                "anything but the JSON is refused, saying why");
    ok &= check(!loupe::editor::parse_scene("{\"objects\": [{\"name\": \"lid\"}]}", "x.scene.toml", &why),
                "an object without its fields is refused");
    const std::string listing = "3 bakes the scene asks for are missing:\n"
                                "packs/capybara/rest.bake\tskin\tab12\tcapybara.scene.toml\n"
                                "packs/capybara/light.bake\tlight\tcd34\tcapybara.scene.toml\n";
    ok &= check(loupe::editor::rebake_rows(listing)
                        == std::vector<std::string>{"packs/capybara/rest.bake", "packs/capybara/light.bake"}
                    && loupe::editor::rebake_rows("nothing is missing\n").empty(),
                "the bake list's rows are its tab-separated lines' first columns; other lines are notes");
    return ok;
}

bool
outliner_tree_is_built_once() {
    loupe::editor::SceneDoc doc;
    doc.objects = {object("box", "", ""), object("lid", "box", ""), object("sun", "", "light"),
                   object("knob", "lid", ""), object("stray", "gone", "")};
    const loupe::window::Editor editor = loupe::window::editor_over(std::move(doc));
    return check(editor.roots == std::vector<size_t>{0, 2, 4} && editor.children[0] == std::vector<size_t>{1}
                     && editor.children[1] == std::vector<size_t>{3} && editor.children[2].empty()
                     && editor.sent.size() == 5,
                 "the outliner's roots and children, an object whose parent is not in the file a root");
}

// sync() tells the engine only what moved, and tries a failed place again.
bool
sync_retries_a_failed_place() {
    loupe::window::Editor editor = loupe::window::editor_over(two_objects().scene);
    std::vector<std::string> placed;
    bool fail = false;
    const loupe::window::PlaceFn place_fn = [&](const std::string& name, const transformf_t&) {
        placed.push_back(name);
        return !fail;
    };
    bool ok = check(loupe::window::sync(editor, place_fn) && placed.empty(), "nothing moved: nothing placed");
    editor.edits.push(place("capybara", at(0), at(1)), editor.docs);
    fail = true;
    ok &= check(!loupe::window::sync(editor, place_fn) && placed == std::vector<std::string>{"capybara"},
                "a failed place says so");
    fail = false;
    placed.clear();
    ok &= check(loupe::window::sync(editor, place_fn) && placed == std::vector<std::string>{"capybara"},
                "the next sync places it again");
    placed.clear();
    ok &= check(loupe::window::sync(editor, place_fn) && placed.empty(), "once placed, it is not placed again");
    editor.edits.undo(editor.docs);
    ok &= check(loupe::window::sync(editor, place_fn) && placed == std::vector<std::string>{"capybara"},
                "an undo places the object once");
    return ok;
}

} // namespace

int
main() {
    bool ok = undo_is_one_stack();
    ok &= what_autana_locks_stays();
    ok &= placement_round_trips();
    ok &= matrices();
    ok &= save_writes_only_what_moved();
    ok &= reads_parse_the_tools_output();
    ok &= outliner_tree_is_built_once();
    ok &= sync_retries_a_failed_place();
    return ok ? 0 : 1;
}
