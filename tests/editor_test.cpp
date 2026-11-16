// The editor's documents without a window: one undo stack across edits, what
// may be placed, the Euler convention both ways against autana's own
// quaternion, the handles' matrices against autana's pinhole, and the save's
// arguments and the read's parse.
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "editor/doc.hpp"
#include "editor/placement.hpp"
#include "editor/scene_tool.hpp"

namespace {

using loupe::editor::Placement;

constexpr const char TEST[] = "editor_test";
constexpr float CLOSE = 1e-4F;

bool
check(bool ok, const std::string& what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what.c_str(), ok ? "ok" : "FAILED");
    return ok;
}

bool
near(float a, float b, float tolerance = CLOSE) {
    return std::fabs(a - b) <= tolerance;
}

loupe::editor::Docs
two_objects() {
    loupe::editor::Docs docs;
    docs.scene.objects = {
        {.path = "capybara", .parent = "", .kind = "skinned_renderer", .local = {}, .saved = {}, .baked = false},
        {.path = "meadow", .parent = "", .kind = "mesh_renderer", .local = {}, .saved = {}, .baked = true},
    };
    return docs;
}

Placement
at(float x) {
    Placement placement;
    placement.position = {x, 0.0F, 0.0F};
    return placement;
}

bool
undo_is_one_stack() {
    loupe::editor::Docs docs = two_objects();
    loupe::editor::EditStack edits;
    bool ok = check(!edits.can_undo() && !edits.can_redo() && !edits.undo(docs), "an empty stack undoes nothing");
    edits.push(std::make_unique<loupe::editor::Place>("capybara", at(0), at(1)), docs);
    edits.push(std::make_unique<loupe::editor::Place>("capybara", at(1), at(2)), docs);
    ok &= check(docs.scene.find("capybara")->local == at(2) && docs.scene.changed.contains("capybara"),
                "a push applies its edit and marks the object changed");
    ok &= check(edits.undo(docs) && docs.scene.find("capybara")->local == at(1), "undo goes back one edit");
    ok &= check(edits.undo(docs) && docs.scene.find("capybara")->local == at(0) && !edits.can_undo(),
                "undo goes back to the start");
    ok &= check(edits.redo(docs) && docs.scene.find("capybara")->local == at(1), "redo comes forward one edit");
    edits.push(std::make_unique<loupe::editor::Place>("capybara", at(1), at(5)), docs);
    ok &= check(!edits.can_redo() && docs.scene.find("capybara")->local == at(5),
                "a push after an undo drops what was undone");
    ok &= check(!edits.last()->needs_rebake(), "a placement needs no rebake");
    return ok;
}

bool
only_unbaked_renderers_place() {
    using loupe::editor::Object;
    const auto object = [](const char* kind, bool baked) {
        return Object{.path = "x", .parent = "", .kind = kind, .local = {}, .saved = {}, .baked = baked};
    };
    return check(loupe::editor::placeable(object("skinned_renderer", false))
                     && loupe::editor::placeable(object("mesh_renderer", false))
                     && !loupe::editor::placeable(object("mesh_renderer", true))
                     && !loupe::editor::placeable(object("light", false))
                     && !loupe::editor::placeable(object("camera", false)),
                 "skinned and unbaked renderers place; baked ones, lights and cameras do not");
}

// A placement whose matrix columns are what autana's quaternion does to the unit axes, scaled.
bool
euler_matches_autana(const Placement& placement) {
    const transformf_t transform = loupe::editor::to_transform(placement);
    const loupe::editor::Matrix matrix = loupe::editor::to_matrix(placement);
    bool ok = true;
    const vec3f_t axes[] = {vec3f_unit_x(), vec3f_unit_y(), vec3f_unit_z()};
    for (int column = 0; column < 3; column++) {
        const vec3f_t turned = quatf_rotate(transform.rotation, axes[column]);
        const float scale = placement.scale[static_cast<size_t>(column)];
        ok &= near(matrix[column * 4 + 0], turned.x * scale) && near(matrix[column * 4 + 1], turned.y * scale)
              && near(matrix[column * 4 + 2], turned.z * scale);
    }
    return ok;
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
        ok &= check(euler_matches_autana(placement), "the matrix turns as autana's quatf_from_euler does");
        const Placement back = loupe::editor::to_placement(loupe::editor::to_matrix(placement));
        bool same = true;
        for (size_t i = 0; i < 3; i++) {
            same &= near(back.position[i], placement.position[i])
                    && near(back.rotation_deg[i], placement.rotation_deg[i], 1e-2F)
                    && near(back.scale[i], placement.scale[i]);
        }
        ok &= check(same, "a placement comes back from its matrix");
    }
    // At pitch 90 yaw and roll turn about one axis: the angles differ, the turn does not.
    const Placement locked = {.position = {}, .rotation_deg = {90.0F, 20.0F, 30.0F}, .scale = {1.0F, 1.0F, 1.0F}};
    const loupe::editor::Matrix again =
        loupe::editor::to_matrix(loupe::editor::to_placement(loupe::editor::to_matrix(locked)));
    const loupe::editor::Matrix first = loupe::editor::to_matrix(locked);
    bool same_turn = true;
    for (size_t i = 0; i < first.size(); i++) {
        same_turn &= near(again[i], first[i]);
    }
    ok &= check(same_turn, "at pitch 90 the decomposed turn is the same turn");
    return ok;
}

bool
matrices() {
    const Placement placement = {
        .position = {1.0F, 2.0F, 3.0F}, .rotation_deg = {10.0F, 20.0F, 30.0F}, .scale = {2.0F, 1.0F, 0.5F}};
    const loupe::editor::Matrix matrix = loupe::editor::to_matrix(placement);
    const loupe::editor::Matrix identity = loupe::editor::multiply(loupe::editor::inverse_affine(matrix), matrix);
    const loupe::editor::Matrix mirrored = loupe::editor::mirror_z(loupe::editor::mirror_z(matrix));
    bool is_identity = true, is_same = true;
    for (int i = 0; i < 16; i++) {
        is_identity &= near(identity[static_cast<size_t>(i)], (i % 5 == 0) ? 1.0F : 0.0F);
        is_same &= near(mirrored[static_cast<size_t>(i)], matrix[static_cast<size_t>(i)]);
    }
    bool ok = check(is_identity, "inverse_affine undoes a placement");
    ok &= check(is_same, "mirror_z twice is no mirror");
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
    loupe::editor::Matrix world{};
    world[0] = world[5] = world[10] = world[15] = 1.0F;
    world[12] = point.x;
    world[13] = point.y;
    world[14] = point.z;
    const loupe::editor::Matrix clip = loupe::editor::multiply(
        loupe::editor::projection_matrix(camera, 1000.0F),
        loupe::editor::multiply(loupe::editor::view_matrix(camera), loupe::editor::mirror_z(world)));
    const float ndc_x = clip[12] / clip[15], ndc_y = clip[13] / clip[15];
    ok &= check(near(ndc_x, 2.0F * px / 368.0F - 1.0F) && near(ndc_y, 1.0F - 2.0F * py / 448.0F) && clip[15] > 0,
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
    const std::vector<std::string> args = loupe::editor::save_args(doc);
    const std::vector<std::string> expected = {"--set", "capybara.position=0.5,0,-0.25", "--set",
                                               "capybara.scale=1.5,1.5,1.5"};
    bool ok = check(args == expected, "the save sets only the fields that moved, in the fewest digits");
    capybara.saved = capybara.local;
    ok &= check(loupe::editor::save_args(doc).empty(), "nothing moved since the save: nothing to set");
    return ok;
}

bool
read_parses_the_tools_json() {
    std::string why;
    const std::string json =
        "a warning first\n{\"objects\": [{\"name\": \"lid\", \"parent\": \"box\", \"kind\": \"mesh_renderer\", "
        "\"baked\": false, \"position\": [0.0, 1.0, 0.0], \"rotation\": [0.0, 90.0, 0.0], \"scale\": [1.0, 1.0, "
        "1.0]}]}\n";
    const std::optional<loupe::editor::SceneDoc> doc = loupe::editor::parse_scene(json, "x.scene.toml", &why);
    bool ok =
        check(doc && doc->objects.size() == 1 && doc->objects[0].parent == "box"
                  && doc->objects[0].local.rotation_deg[1] == 90.0F && doc->objects[0].saved == doc->objects[0].local,
              "the read's JSON line becomes the document, saved as read");
    ok &= check(!loupe::editor::parse_scene("scene_edit: refused\n", "x.scene.toml", &why) && !why.empty(),
                "anything but the JSON is refused, saying why");
    ok &= check(!loupe::editor::parse_scene("{\"objects\": [{\"name\": \"lid\"}]}", "x.scene.toml", &why),
                "an object without its fields is refused");
    return ok;
}

} // namespace

int
main() {
    bool ok = undo_is_one_stack();
    ok &= only_unbaked_renderers_place();
    ok &= placement_round_trips();
    ok &= matrices();
    ok &= save_writes_only_what_moved();
    ok &= read_parses_the_tools_json();
    return ok ? 0 : 1;
}
