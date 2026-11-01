#include "engine/frames.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <map>
#include <vector>

#include "engine/autana.h"
#include "pack.h"

namespace loupe {

struct Frames::State {
    std::vector<std::string> args;
    std::vector<char*> argv;
    bool open = false;
    bool keep = false;
    int index = 0;
    uint32_t dt_ms = 0;                                   // the run's, as it opened
    std::vector<uint32_t> dts;                            // each drawn frame's dt, for replaying it
    std::map<int, render_host_checkpoint_t*> checkpoints; // the state before drawing frame key

    // Frees the checkpoints after frame `frame` and forgets the dts from it on.
    void
    forget_after(int frame) {
        for (auto it = checkpoints.upper_bound(frame); it != checkpoints.end(); it = checkpoints.erase(it)) {
            render_host_checkpoint_free(it->second);
        }
        dts.resize(static_cast<size_t>(std::max(frame, 0)));
    }
};

namespace {

bool
set_asset_dir(const std::string& folder) {
#if defined(_WIN32)
    return _putenv_s(ASSET_STORE_DIR_ENV, folder.c_str()) == 0;
#else
    return setenv(ASSET_STORE_DIR_ENV, folder.c_str(), 1) == 0;
#endif
}

// The scene provider's command line for `options`: only the flags given, as scene_viewer.sh passes them.
std::vector<std::string>
provider_args(const Options& options) {
    std::vector<std::string> args = {"loupe", "--scene", scene_id(options.scene_toml)};
    const auto add = [&args](const char* flag, const std::string& value) {
        if (!value.empty()) {
            args.insert(args.end(), {flag, value});
        }
    };
    for (const std::string& object : options.objects) {
        add("--object", object);
    }
    add("--camera", options.camera);
    add("--view", options.view);
    add("--frames", options.frames ? std::to_string(*options.frames) : "");
    add("--dt", options.dt_ms ? std::to_string(*options.dt_ms) : "");
    for (const std::string& set : options.sets) {
        add("--set", set);
    }
    add("-o", options.out_bmp);
    add("--video", options.out_video);
    return args;
}

} // namespace

Frames::Frames(std::unique_ptr<State> state) : state_(std::move(state)) {}

std::unique_ptr<Frames>
Frames::open(const Options& options, std::string* why) {
    const std::optional<std::string> pack = scene_pack(options.scene_toml, why);
    if (!pack) {
        return nullptr;
    }
    if (!set_asset_dir(*pack)) {
        *why = "cannot set " ASSET_STORE_DIR_ENV;
        return nullptr;
    }
    auto state = std::make_unique<State>();
    state->args = provider_args(options);
    for (std::string& arg : state->args) {
        state->argv.push_back(arg.data());
    }
    state->argv.push_back(nullptr);
    int exit_code = 0;
    if (!render_host_open(&scene_provider, static_cast<int>(state->args.size()), state->argv.data(), &exit_code)) {
        *why = "the scene did not open; autana said why above";
        return nullptr;
    }
    state->open = true;
    state->dt_ms = render_host_frame()->dt_ms;
    return std::unique_ptr<Frames>(new Frames(std::move(state)));
}

Frames::~Frames() { (void)close(); }

int
Frames::count() const {
    return render_host_frame()->count;
}

uint32_t
Frames::dt_ms() const {
    return state_->dt_ms;
}

void
Frames::keep_checkpoints() {
    state_->keep = true;
}

bool
Frames::step(uint32_t dt_ms) {
    State& s = *state_;
    if (s.index < std::ssize(s.dts) && s.dts[s.index] != dt_ms) {
        s.forget_after(s.index);
    }
    bool saved = true;
    // From frame 1: autana restores only state whose allocations the first frame made.
    if (s.keep && s.index % CHECKPOINT_EVERY == 1 && !s.checkpoints.contains(s.index)) {
        render_host_checkpoint_t* checkpoint = render_host_save();
        saved = checkpoint != nullptr;
        if (saved) {
            s.checkpoints[s.index] = checkpoint;
        } else {
            std::fprintf(stderr, "loupe: no checkpoint before frame %d; seeks back past it will fail\n", s.index);
        }
    }
    const input_t input = {};
    render_host_step(dt_ms, &input);
    if (s.index == std::ssize(s.dts)) {
        s.dts.push_back(dt_ms);
    }
    s.index++;
    return saved;
}

bool
Frames::seek(int index) {
    State& s = *state_;
    if (index < 0) {
        return false;
    }
    if (index < s.index - 1) {
        auto from = s.checkpoints.upper_bound(index);
        if (from == s.checkpoints.begin()) {
            return false;
        }
        --from;
        if (!render_host_restore(from->second)) {
            return false;
        }
        s.index = from->first;
    }
    while (s.index <= index) {
        step(s.index < std::ssize(s.dts) ? s.dts[s.index] : dt_ms());
    }
    return true;
}

int
Frames::index() const {
    return state_->index;
}

Frame
Frames::frame() const {
    size_t bytes = 0;
    const uint8_t* pixels = render_host_pixels(&bytes);
    return Frame(pixels, bytes);
}

std::optional<uint32_t>
Frames::path_ms() const {
    if (scene_provider.path_ms == nullptr) {
        return std::nullopt;
    }
    return scene_provider.path_ms();
}

bool
Frames::close() {
    if (!state_->open) {
        return true;
    }
    state_->forget_after(-1);
    state_->open = false;
    return render_host_close();
}

} // namespace loupe
