#include "engine/frames.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include "engine/autana.hpp"
#include "pack.hpp"
#include "paths.hpp"

namespace loupe {

struct Frames::State {
    std::string id; // the scene's
    std::vector<std::string> args;
    std::vector<char*> argv;
    bool open = false;
    bool keep = false;
    size_t budget = 0; // the checkpoints' memory, at most
    int every = CHECKPOINT_EVERY;
    size_t kept_bytes = 0;
    bool said_over = false;
    int index = 0;
    uint32_t dt_ms = 0;                                   // the run's, as it opened
    std::vector<uint32_t> dts;                            // each drawn frame's dt, for replaying it
    std::map<int, render_host_checkpoint_t*> checkpoints; // the state before drawing frame key

    // Checkpoints sit before frames FIRST_SEEKABLE, FIRST_SEEKABLE + every...
    bool
    on_grid(int frame) const {
        return (frame - Frames::FIRST_SEEKABLE) % every == 0;
    }

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
                it = on_grid(it->first) ? std::next(it) : drop(it);
            }
        }
        if (kept_bytes > budget && !said_over) {
            said_over = true;
            std::fprintf(stderr, "loupe: one checkpoint holds %zu bytes, over the %zu-byte budget\n", kept_bytes,
                         budget);
        }
    }
};

namespace {

bool
set_asset_dir(const std::filesystem::path& folder) {
#if defined(_WIN32)
    return _putenv_s(ASSET_STORE_DIR_ENV, utf8(folder).c_str()) == 0;
#else
    return setenv(ASSET_STORE_DIR_ENV, folder.c_str(), 1) == 0;
#endif
}

// The scene provider's command line for scene `id` and `options`: only the
// flags given, as scene_viewer.sh passes them.
std::vector<std::string>
provider_args(const std::string& id, const Options& options) {
    std::vector<std::string> args = {"loupe", "--scene", id};
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
    const std::optional<ScenePack> pack = resolve_scene(options.scene, why);
    if (!pack) {
        return nullptr;
    }
    if (!set_asset_dir(pack->folder)) {
        *why = "cannot set " ASSET_STORE_DIR_ENV;
        return nullptr;
    }
    auto state = std::make_unique<State>();
    state->id = pack->id;
    state->args = provider_args(pack->id, options);
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

const std::string&
Frames::scene_id() const {
    return state_->id;
}

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
    render_context_set_debug_view(scene_provider.render_context(), view);
    return this->view() == view && redraw_shown();
}

bool
Frames::step(uint32_t dt_ms) {
    State& s = *state_;
    if (s.index < std::ssize(s.dts) && s.dts[s.index] != dt_ms) {
        s.forget_after(s.index);
    }
    bool saved = true;
    // From frame 1: autana restores only state whose allocations the first frame made.
    if (s.keep && s.on_grid(s.index) && !s.checkpoints.contains(s.index)) {
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
    if (index < 0 || (index < state_->index - 1 && !restore_before(index))) {
        return false;
    }
    step_to(index);
    return true;
}

bool
Frames::restore_before(int index) {
    State& s = *state_;
    auto from = s.checkpoints.upper_bound(index);
    if (from == s.checkpoints.begin()) {
        return false;
    }
    --from;
    // The checkpoint holds the view it was saved with; the view picked now stays.
    const int picked = view();
    if (!render_host_restore(from->second)) {
        return false;
    }
    if (has_views()) {
        render_context_set_debug_view(scene_provider.render_context(), picked);
    }
    s.index = from->first;
    return true;
}

void
Frames::step_to(int index) {
    State& s = *state_;
    while (s.index <= index) {
        step(s.index < std::ssize(s.dts) ? s.dts[s.index] : dt_ms());
    }
}

bool
Frames::redraw_shown() {
    const int shown = state_->index - 1;
    // Nothing drawn yet, or no checkpoint before the shown frame: the next step draws under the view.
    if (shown < 0 || state_->checkpoints.upper_bound(shown) == state_->checkpoints.begin()) {
        return true;
    }
    if (!restore_before(shown)) {
        return false;
    }
    step_to(shown);
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
    std::vector<DebugView> views;
    for (int id = RENDER_DEBUG_VIEW_SHADED; id <= RENDER_DEBUG_VIEW_COUNT; id++) {
        if (const char* name = render_context_debug_view_name(id)) {
            views.push_back({.id = id, .name = name});
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
