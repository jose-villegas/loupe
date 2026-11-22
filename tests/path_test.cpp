// A camera path in the engine: the curve the editor draws from the keys file
// (read through autana's scene_edit.py) is where the scene's own clip puts the
// camera at every frame, past the path's end too; and a camera flown on an
// edited curve draws from it on every frame, sought or stepped, until landed,
// when every frame is the clip's again, byte for byte.
//
//   path_test SCENE.apak OUT.bmp CAMERA.keys.toml CAMERA
#include <cmath>
#include <cstdio>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "checks.hpp"
#include "editor/camera_path.hpp"
#include "editor/scene_tool.hpp"
#include "engine/engine_frame.hpp"
#include "frames_fixture.hpp"
#include "scratch.hpp"

namespace {

constexpr const char TEST[] = "path_test";
// Frames this far apart cover a 45 s path and its loop back in a short run.
constexpr uint32_t DT_MS = 250;
// Past the path's end by this many frames, where the clip plays round.
constexpr int PAST_END = 8;
// Eye and forward against the clip's, relative to the scene's size: float maths on two routes.
constexpr float CLOSE = 2e-4F;
constexpr float CLOSE_TURN = 1e-4F;
// How far the edited key's eye moves, in scene units.
constexpr float NUDGE = 40.0F;

constexpr loupe::test::Checks check{TEST};

float
gap(const vec3f_t& a, const vec3f_t& b) {
    return std::hypot(a.x - b.x, a.y - b.y, a.z - b.z);
}

// Whether the shown frame's camera stands at `pose` (relative to `camera`'s parent), eye and facing.
bool
at_pose(const loupe::Frames& frames, const char* camera, const transformf_t& pose, float scale) {
    const std::optional<loupe::ViewCamera> drew = frames.view_camera();
    const std::optional<mat4f_t> parent = frames.parent_world(camera);
    if (!drew || !parent) {
        return false;
    }
    transformf_t local = pose; // transformf_matrix caches the matrix in it
    const mat4f_t world = mat4f_mul(*parent, transformf_matrix(&local));
    const vec3f_t eye = {world.m[0][3], world.m[1][3], world.m[2][3]};
    // In the engine frame a camera looks down its local +Z (render_view_make).
    const vec3f_t forward = vec3f_normalize(vec3f_t{world.m[0][2], world.m[1][2], world.m[2][2]});
    const float off_eye = gap(drew->eye, eye), off_turn = gap(drew->forward, forward);
    const bool close = off_eye <= CLOSE * scale && off_turn <= CLOSE_TURN;
    if (!close) {
        std::fprintf(stderr, "%s: the eye is %g off and the forward %g\n", TEST, static_cast<double>(off_eye),
                     static_cast<double>(off_turn));
    }
    return close;
}

float
size_of(const std::vector<loupe::editor::CameraKey>& keys) {
    float most = 1.0F;
    for (const loupe::editor::CameraKey& key : keys) {
        for (const float value : key.eye) {
            most = std::fmax(most, std::fabs(value));
        }
    }
    return most;
}

} // namespace

int
main(int argc, char** argv) {
    const std::span<char*> args(argv, static_cast<size_t>(argc));
    if (args.size() != 5) {
        std::fprintf(stderr, "usage: %s SCENE.apak OUT.bmp CAMERA.keys.toml CAMERA\n", TEST);
        return 1;
    }
    const loupe::Scratch scratch("loupe-path-test-");
    std::string why;
    const std::optional<loupe::editor::CameraPath> path = loupe::editor::read_path(args[3], scratch.path(), &why);
    if (!path) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
        return 1;
    }
    const loupe::PathCurve curve = loupe::editor::path_curve(path->keys);
    const float end_s = loupe::editor::path_end(path->keys);
    const float scale = size_of(path->keys);
    const int frames_on_path = static_cast<int>(std::lround(end_s * 1000.0F / DT_MS)) + PAST_END;

    loupe::Options options;
    options.scene = args[1];
    options.out_bmp = args[2];
    options.render.camera = args[4];
    options.dt_ms = DT_MS;
    options.frames = frames_on_path + 1;
    const std::unique_ptr<loupe::Frames> frames = loupe::test::open_or_report(TEST, options);
    if (frames == nullptr) {
        return 1;
    }
    frames->keep_checkpoints(SIZE_MAX);
    bool ok = check(frames->is_active(args[4]) && !frames->is_active("no_such_object"),
                    std::string("the active camera is ") + args[4]);

    // The clip's own camera at every frame, against the curve at that frame's time.
    loupe::test::Straight straight;
    int off = -1;
    ok &= loupe::test::step_straight(*frames, frames_on_path + 1, {}, &straight, [&](int shown) {
        const float seconds = loupe::editor::clip_seconds(path->keys, frames->time_ms(shown));
        if (!at_pose(*frames, args[4], loupe::engine_frame(loupe::pose_on(curve, seconds)), scale) && off < 0) {
            off = shown;
        }
        return true;
    });
    ok &= check(off < 0, "the curve is where the clip puts the camera at every frame"
                             + (off < 0 ? std::string() : ", not at frame " + std::to_string(off)));

    // Fly the camera on the curve with one key's eye moved.
    const size_t moved = path->keys.size() / 2;
    std::vector<loupe::editor::CameraKey> edited = path->keys;
    edited[moved].eye[1] += NUDGE;
    const loupe::PathCurve flown = loupe::editor::path_curve(edited);
    const auto pose_at = [&flown, &edited](uint32_t t_ms) {
        return loupe::engine_frame(loupe::pose_on(flown, loupe::editor::clip_seconds(edited, t_ms)));
    };
    const int key_frame = static_cast<int>(std::lround(edited[moved].t * 1000.0F / DT_MS));
    ok &= frames->seek(key_frame);
    const uint32_t key_ms = frames->time_ms(key_frame);
    ok &= check(frames->fly(args[4], pose_at), "the camera flies the edited curve");
    ok &= check(frames->index() - 1 == key_frame && at_pose(*frames, args[4], pose_at(key_ms), scale)
                    && loupe::test::pixels(*frames) != straight[key_frame],
                "the shown frame is drawn again from the edited curve");
    ok &= check(!frames->fly("no_such_object", pose_at), "an unknown camera is refused");
    const int later = key_frame + 3;
    while (frames->index() <= later) {
        ok &= frames->step(frames->dt_ms());
    }
    ok &= check(at_pose(*frames, args[4], pose_at(frames->time_ms(later)), scale),
                "a later frame stepped to flies the curve");
    const std::vector<uint8_t> flown_later = loupe::test::pixels(*frames);
    // A key's move bends the curve from two keys before it (Catmull-Rom tangents); earlier frames stay the clip's.
    if (moved >= 3) {
        const int early = static_cast<int>(std::lround(edited[moved - 3].t * 1000.0F / DT_MS));
        ok &= frames->seek(early);
        ok &= check(at_pose(*frames, args[4],
                            loupe::engine_frame(
                                loupe::pose_on(curve, loupe::editor::clip_seconds(path->keys, frames->time_ms(early)))),
                            scale),
                    "flying, a frame the move does not reach is where the clip puts the camera");
    }
    ok &= loupe::test::same(TEST, *frames, later, flown_later, "flying, sought back to the later frame");

    ok &= check(frames->land(args[4]), "the camera lands back on its clip");
    const std::vector<uint8_t> landed = loupe::test::pixels(*frames);
    ok &= check(frames->land(args[4]) && frames->land("no_such_object") && loupe::test::pixels(*frames) == landed,
                "a camera not flying lands as it is");
    ok &= loupe::test::same(TEST, *frames, later, straight[later], "landed, the later frame is the clip's");
    ok &= loupe::test::same(TEST, *frames, key_frame, straight[key_frame], "landed, the key's frame is the clip's");
    while (frames->index() <= frames_on_path) {
        ok &= frames->step(frames->dt_ms());
    }
    ok &= check(loupe::test::pixels(*frames) == straight[frames_on_path], "landed, stepped on, the clip's frames");
    return frames->close() && ok ? 0 : 1;
}
