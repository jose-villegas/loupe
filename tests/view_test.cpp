// Writes one frame's bytes for view_test.cmake to compare. "straight VIEW": the
// run opens under --view VIEW and steps to the frame. "switched": the run steps
// shaded past the frame, then for every view picks it and seeks back, which
// restores a checkpoint and sets the view again. The two agree only when
// picking a view mid-run draws what that view drew from the start.
#include <cstdio>
#include <fstream>
#include <limits>
#include <string>

#include "engine/frames.hpp"

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

} // namespace

int
main(int argc, char** argv) {
    const std::string mode = argc >= 4 ? argv[3] : "";
    if (argc != (mode == "straight" ? 5 : 4) || (mode != "straight" && mode != "switched")) {
        std::fprintf(stderr, "usage: view_test SCENE.scene.toml OUT_DIR straight VIEW | switched\n");
        return 2;
    }
    const std::string out = argv[2];
    loupe::Options options;
    options.scene_toml = argv[1];
    options.out_bmp = out + "/" + mode + ".bmp";
    if (mode == "straight") {
        options.view = argv[4];
    }
    std::string why;
    std::unique_ptr<loupe::Frames> frames = loupe::Frames::open(options, &why);
    if (frames == nullptr) {
        std::fprintf(stderr, "view_test: %s\n", why.c_str());
        return 1;
    }
    bool ok = frames->has_views();
    if (mode == "straight") {
        while (frames->index() <= FRAME) {
            ok &= frames->step(frames->dt_ms());
        }
        ok &= write(*frames, out + "/straight-" + argv[4] + ".bin");
        return frames->close() && ok ? 0 : 1;
    }

    frames->keep_checkpoints(std::numeric_limits<size_t>::max());
    while (frames->index() <= SWITCH_AT) {
        ok &= frames->step(frames->dt_ms());
    }
    std::ofstream names(out + "/views.txt");
    for (const loupe::DebugView& view : loupe::debug_views()) {
        const bool sought = frames->set_view(view.id) && frames->view() == view.id && frames->seek(FRAME);
        if (!sought || frames->view() != view.id) {
            std::fprintf(stderr, "view_test: %s did not stay picked through the seek\n", view.name.c_str());
            ok = false;
        }
        ok &= write(*frames, out + "/switched-" + view.name + ".bin");
        names << view.name << '\n';
    }
    return frames->close() && ok ? 0 : 1;
}
