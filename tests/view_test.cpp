// Writes two frames' bytes for view_test.cmake to compare. "straight VIEW": the
// run opens under --view VIEW and steps to SWITCH_AT, writing FRAME on the way.
// "switched": the run steps shaded to SWITCH_AT, then for every view picks it,
// which draws SWITCH_AT again under it, and seeks back to FRAME, which restores
// a checkpoint and sets the view again. They agree only when picking a view
// mid-run draws what that view drew from the start.
#include <cstdio>
#include <format>
#include <fstream>
#include <limits>
#include <string>

#include "frames_fixture.hpp"

namespace {

constexpr int EVERY = loupe::Frames::CHECKPOINT_EVERY;
// Past the second checkpoint, so the seek back restores one drawn shaded.
constexpr int FRAME = EVERY + 10;
constexpr int SWITCH_AT = FRAME + 10;

bool
write(const loupe::Frames& frames, const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    const loupe::Frame frame = frames.frame();
    out.write(reinterpret_cast<const char*>(frame.bgr.data()), static_cast<std::streamsize>(frame.bgr.size()));
    return out.good();
}

// Under `view` from the start: writes FRAME and SWITCH_AT.
bool
straight(loupe::Frames& frames, const std::string& out, const std::string& view) {
    bool ok = true;
    while (frames.index() <= SWITCH_AT) {
        ok &= frames.step(frames.dt_ms());
        if (frames.index() - 1 == FRAME) {
            ok &= write(frames, std::format("{}/straight-{}-sought.bin", out, view));
        }
    }
    return write(frames, std::format("{}/straight-{}-picked.bin", out, view)) && ok;
}

// Shaded to SWITCH_AT, then each view picked there and sought back to FRAME; names the views in views.txt.
bool
switched(loupe::Frames& frames, const std::string& out) {
    frames.keep_checkpoints(std::numeric_limits<size_t>::max());
    bool ok = true;
    while (frames.index() <= SWITCH_AT) {
        ok &= frames.step(frames.dt_ms());
    }
    std::ofstream names(std::format("{}/views.txt", out));
    for (const loupe::DebugView& view : loupe::debug_views()) {
        const bool picked = frames.seek(SWITCH_AT) && frames.set_view(view.id);
        ok &= picked && write(frames, std::format("{}/switched-{}-picked.bin", out, view.name));
        const bool sought = picked && frames.seek(FRAME) && frames.view() == view.id;
        if (!sought) {
            std::fprintf(stderr, "view_test: %s did not stay picked through the seek\n", view.name.c_str());
            ok = false;
        }
        ok &= write(frames, std::format("{}/switched-{}-sought.bin", out, view.name));
        names << view.name << '\n';
    }
    return ok;
}

} // namespace

int
main(int argc, char** argv) {
    const std::string mode = argc >= 4 ? argv[3] : "";
    const bool is_straight = mode == "straight";
    if (argc != (is_straight ? 5 : 4) || (!is_straight && mode != "switched")) {
        std::fprintf(stderr, "usage: view_test SCENE.apak OUT_DIR straight VIEW | switched\n");
        return 2;
    }
    const std::string out = argv[2];
    loupe::Options options;
    options.scene = argv[1];
    options.out_bmp = std::format("{}/{}.bmp", out, mode);
    options.view = is_straight ? argv[4] : "";
    std::unique_ptr<loupe::Frames> frames = loupe::test::open_or_report("view_test", options);
    if (frames == nullptr) {
        return 1;
    }
    const bool ok =
        frames->has_views() && (is_straight ? straight(*frames, out, options.view) : switched(*frames, out));
    return frames->close() && ok ? 0 : 1;
}
