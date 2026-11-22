// Frames::place: a placed object draws where it was put on the shown frame at
// once, and on every frame after, sought or stepped; placed back where the
// scene had it, every frame is the straight run's again. After the first, a
// place redraws in one engine frame, as a drag needs. An unknown name is
// refused. view_camera() frames the shown frame, with the active camera's own
// axes: the shown frame is the panel's picture turned back upright.
#include <cmath>
#include <cstdio>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include "checks.hpp"
#include "editor/doc.hpp"
#include "editor/placement.hpp"
#include "frames_fixture.hpp"

namespace {

constexpr const char TEST[] = "place_test";
// Every sponza renderer: whichever is drawn moves.
const char* const RENDERERS[] = {"atrium_fitted_full", "atrium_fitted", "atrium_flat_fitted", "atrium",
                                 "atrium_lite",        "atrium_flat"};
constexpr int EVERY = loupe::Frames::CHECKPOINT_EVERY;
constexpr int SHOWN = EVERY + 10;
constexpr int LATER = SHOWN + 5;
// How far the renderers move: a tenth of a metre up, then a drag's next step.
constexpr float LIFT = 0.1F;
constexpr float NUDGE = 0.01F;
constexpr float CLOSE = 1e-4F;

constexpr loupe::test::Checks check{TEST};

// Sponza's camera has no roll: its right stays level and its up points up, in the picture as in the world.
bool
level(const loupe::ViewCamera& camera) {
    return std::fabs(camera.right.y) <= CLOSE && camera.up.y > 0.0F;
}

// Every renderer at `height` above where the scene has it.
bool
place_all(loupe::Frames& frames, float height) {
    loupe::editor::Placement placement;
    placement.position = {0.0F, height, 0.0F};
    bool ok = true;
    for (const char* name : RENDERERS) {
        ok &= frames.place(name, loupe::editor::to_transform(placement));
    }
    return ok;
}

} // namespace

int
main(int argc, char** argv) {
    loupe::test::Straight straight;
    bool ok = false;
    const std::unique_ptr<loupe::Frames> frames =
        loupe::test::open_stepped(TEST, std::span(argv, static_cast<size_t>(argc)), LATER + 1, &straight, &ok);
    if (frames == nullptr) {
        return 1;
    }
    ok &= frames->seek(SHOWN);
    ok &= check(!frames->place("no_such_object", transformf_identity()), "an unknown object is refused");
    ok &= check(!frames->parent_world("no_such_object").has_value(), "an unknown object has no parent");
    const std::optional<loupe::ViewCamera> camera = frames->view_camera();
    ok &= check(camera && camera->width == frames->frame().width && camera->height == frames->frame().height,
                "the view camera frames the shown frame");
    ok &= check(camera && level(*camera), "the view camera's right is level and its up points up, as sponza's camera");

    // Sponza's renderers are baked, so the scene has them at identity.
    const int64_t before = frames->drawn();
    ok &= check(place_all(*frames, LIFT), "every renderer places");
    ok &= check(frames->index() - 1 == SHOWN && frames->drawn() > before, "the shown frame is drawn again");
    const std::vector<uint8_t> placed = loupe::test::pixels(*frames);
    ok &= check(placed != straight[SHOWN], "the placed frame differs from the straight run's");
    const int64_t held = frames->drawn();
    ok &= check(place_all(*frames, LIFT + NUDGE) && frames->drawn() - held == std::ssize(RENDERERS),
                "each place after the first draws one engine frame");
    ok &= check(place_all(*frames, LIFT), "the renderers place back to the first lift");
    ok &= check(loupe::test::pixels(*frames) == placed, "placed back to the same lift, the same frame");

    while (frames->index() <= LATER) {
        ok &= frames->step(frames->dt_ms());
    }
    const std::vector<uint8_t> stepped_later = loupe::test::pixels(*frames);
    ok &= check(stepped_later != straight[LATER], "a later frame, stepped to, draws the placement");
    ok &= loupe::test::same(TEST, *frames, LATER, stepped_later, "the later frame sought to is the one stepped to");
    ok &= loupe::test::same(TEST, *frames, SHOWN, placed, "placed, sought back to");

    ok &= check(place_all(*frames, 0.0F), "every renderer places back");
    ok &= loupe::test::same(TEST, *frames, SHOWN, straight[SHOWN], "placed back");
    ok &= loupe::test::same(TEST, *frames, LATER, straight[LATER], "placed back, later");
    return frames->close() && ok ? 0 : 1;
}
