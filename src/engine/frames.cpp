#include "engine/frames.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <map>
#include <vector>

#include "engine/autana.hpp"
#include "pack.hpp"

namespace loupe {

struct Frames::State {
    std::vector<std::string> args;
    std::vector<char*> argv;
    bool open = false;
    bool keep = false;
    size_t budget = 0; // the checkpoints' memory, at most
    int every = CHECKPOINT_EVERY;
    size_t kept_bytes = 0;
    int index = 0;
    uint32_t dt_ms = 0;                                   // the run's, as it opened
    std::vector<uint32_t> dts;                            // each drawn frame's dt, for replaying it
    std::map<int, render_host_checkpoint_t*> checkpoints; // the state before drawing frame key
    std::optional<int> view;                              // the picked view, set again after each restore

    std::map<int, render_host_checkpoint_t*>::iterator
    drop(std::map<int, render_host_checkpoint_t*>::iterator it) {
        kept_bytes -= render_host_checkpoint_bytes(it->second);
        render_host_checkpoint_free(it->second);
        return checkpoints.erase(it);
    }

    // Frees the checkpoints after frame `frame` and forgets the dts from it on.
    void
    forget_after(int frame) {
        for (auto it = checkpoints.upper_bound(frame); it != checkpoints.end();) {
            it = drop(it);
        }
        dts.resize(static_cast<size_t>(std::max(frame, 0)));
    }

    // Past the budget, keeps every other checkpoint, so the interval doubles and coverage stays even.
    void
    thin() {
        while (kept_bytes > budget && checkpoints.size() > 1) {
            every *= 2;
            for (auto it = checkpoints.begin(); it != checkpoints.end();) {
                it = (it->first - 1) % every == 0 ? std::next(it) : drop(it);
            }
        }
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
Frames::keep_checkpoints(size_t budget_bytes) {
    state_->keep = true;
    state_->budget = budget_bytes;
    state_->thin();
}

int
Frames::checkpoint_every() const {
    return state_->every;
}

size_t
Frames::checkpoint_bytes() const {
    return state_->kept_bytes;
}

bool
Frames::has_views() const {
    return scene_provider.render_context != nullptr;
}

int
Frames::view() const {
    return has_views() ? scene_provider.render_context()->debug_view : RENDER_DEBUG_VIEW_SHADED;
}

bool
Frames::set_view(int view) {
    if (!has_views()) {
        return false;
    }
    state_->view = view;
    render_context_set_debug_view(scene_provider.render_context(), view);
    if (state_->index > 0) {
        (void)replay(state_->index - 1, true);
    }
    return true;
}

bool
Frames::step(uint32_t dt_ms) {
    State& s = *state_;
    if (s.index < std::ssize(s.dts) && s.dts[s.index] != dt_ms) {
        s.forget_after(s.index);
    }
    bool saved = true;
    // From frame 1: autana restores only state whose allocations the first frame made.
    if (s.keep && s.index % s.every == 1 && !s.checkpoints.contains(s.index)) {
        render_host_checkpoint_t* checkpoint = render_host_save();
        saved = checkpoint != nullptr;
        if (saved) {
            s.checkpoints[s.index] = checkpoint;
            s.kept_bytes += render_host_checkpoint_bytes(checkpoint);
            s.thin();
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
    return replay(index, false);
}

bool
Frames::replay(int index, bool redraw) {
    State& s = *state_;
    if (index < 0) {
        return false;
    }
    if (index < s.index - 1 || (redraw && index == s.index - 1)) {
        auto from = s.checkpoints.upper_bound(index);
        if (from == s.checkpoints.begin()) {
            return false;
        }
        --from;
        if (!render_host_restore(from->second)) {
            return false;
        }
        if (s.view) {
            render_context_set_debug_view(scene_provider.render_context(), *s.view);
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
    const render_host_image_t image = render_host_image();
    return {.bgr = std::span(image.bgr, image.stride * static_cast<size_t>(image.height)),
            .width = image.width,
            .height = image.height,
            .stride = image.stride};
}

std::vector<DebugView>
debug_views() {
    std::vector<DebugView> views = {{.id = RENDER_DEBUG_VIEW_SHADED, .name = "shaded"}};
    for (int id = RENDER_DEBUG_VIEW_SHADED + 1; id <= RENDER_DEBUG_VIEW_COUNT; id++) {
        if (const render_debug_view_t* row = render_context_debug_view(id)) {
            views.push_back({.id = id, .name = row->name});
        }
    }
    return views;
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
