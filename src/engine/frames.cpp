#include "engine/frames.hpp"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "engine/autana.hpp"
#include "pack.hpp"
#include "paths.hpp"
#include "process.hpp"

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
    // While observing or placing: the state before drawing the shown frame,
    // apart from the kept checkpoints, which thinning may drop.
    render_host_checkpoint_t* held = nullptr;
    int64_t drawn = 0; // engine frames, replays included
    // Objects the editor placed, by name: set on the scene before every frame is drawn.
    std::map<std::string, transformf_t> placed;
    // A stream (open_stream): every frame is drawn from its keyframe, so held
    // and the checkpoints stay empty, and an observer is redrawn as a seek.
    bool stream = false, live = false, observing = false;

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

// Every tunable back to its own default. The registry outlives a run, so a
// --set of the run before would otherwise reach this one.
void
reset_tunables() {
    tune_registry_t* registry = tune_shared();
    for (const tune_entry_t* entry = registry->first; entry != nullptr; entry = entry->next) {
        (void)tune_reset(registry, entry->name);
    }
}

// The scene provider's command line for scene `id` and `options`: only the
// flags given, as scene_viewer.sh passes them.
std::vector<std::string>
provider_args(const std::string& id, const Options& options) {
    std::vector<std::string> args = {"loupe", "--scene", id};
    const std::vector<std::string> render = options.render.args();
    args.insert(args.end(), render.begin(), render.end());
    const auto add = [&args](const char* flag, const std::string& value) {
        if (!value.empty()) {
            args.insert(args.end(), {flag, value});
        }
    };
    add("--frames", options.frames ? std::to_string(*options.frames) : "");
    add("--dt", options.dt_ms ? std::to_string(*options.dt_ms) : "");
    add("-o", options.out_bmp);
    add("--video", options.out_video);
    add("--record", options.record);
    return args;
}

// The provider of the open run: one Frames is open at a time.
const frame_provider_t* provider = &scene_provider;

// The render context the provider draws through; null for one that draws without.
render_context_t*
context() {
    return provider->render_context != nullptr ? provider->render_context() : nullptr;
}

// Restores `checkpoint`. It holds the view it was saved with; the view picked now stays.
bool
restore_keeping_view(render_host_checkpoint_t* checkpoint) {
    render_context_t* drawing = context();
    const int picked = drawing != nullptr ? drawing->debug_view : RENDER_DEBUG_VIEW_SHADED;
    if (!render_host_restore(checkpoint)) {
        return false;
    }
    if (drawing != nullptr) {
        render_context_set_debug_view(drawing, picked);
    }
    return true;
}

// The scene object named `name` in the scene the active camera belongs to; none without a camera or such an object.
entity_t
entity_named(const std::string& name) {
    entity_t root = camera_active();
    if (handle_is_none(root)) {
        return entity_t{}; // ENTITY_NONE, the zero handle, without its C compound literal
    }
    for (entity_t up = entity_parent(root); !handle_is_none(up); up = entity_parent(up)) {
        root = up;
    }
    return entity_find(root, name_hash(name.c_str()));
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
    state->argv = argv_of(state->args);
    int exit_code = 0;
    reset_tunables();
    provider = &scene_provider;
    if (!render_host_open(&scene_provider, static_cast<int>(state->args.size()), state->argv.data(), &exit_code)) {
        *why = "the scene did not open; autana said why above";
        return nullptr;
    }
    state->open = true;
    state->dt_ms = render_host_frame()->dt_ms;
    return std::unique_ptr<Frames>(new Frames(std::move(state)));
}

std::unique_ptr<Frames>
Frames::open_stream(const Options& options, const std::filesystem::path& packs, std::string id, std::string* why) {
    if (!set_asset_dir(packs)) {
        *why = "cannot set " ASSET_STORE_DIR_ENV;
        return nullptr;
    }
    auto state = std::make_unique<State>();
    state->id = std::move(id);
    state->stream = true;
    state->live = options.scene.empty();
    state->args = {"loupe"};
    if (state->live) {
        // No frame yet: the host refuses a run of none, and the stream's own count is read live.
        stream_forget();
        state->args.insert(state->args.end(), {"--frames", "1"});
    } else {
        state->args.insert(state->args.end(), {"--stream", utf8(options.scene)});
    }
    if (!options.out_bmp.empty()) {
        state->args.insert(state->args.end(), {"-o", options.out_bmp});
    }
    state->argv = argv_of(state->args);
    int exit_code = 0;
    reset_tunables();
    provider = &stream_provider;
    if (!render_host_open(&stream_provider, static_cast<int>(state->args.size()), state->argv.data(), &exit_code)) {
        const stream_status_t status = stream_status();
        *why = status.refused ? status.why : "the stream did not open; autana said why above";
        return nullptr;
    }
    state->open = true;
    return std::unique_ptr<Frames>(new Frames(std::move(state)));
}

Frames::~Frames() { (void)close(); }

const std::string&
Frames::scene_id() const {
    return state_->id;
}

int
Frames::count() const {
    return state_->stream ? stream_status().frames : render_host_frame()->count;
}

uint32_t
Frames::dt_ms() const {
    return state_->stream ? stream_frame_info(state_->index).dt_ms : state_->dt_ms;
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
    return context() != nullptr;
}

int
Frames::view() const {
    return has_views() ? context()->debug_view : RENDER_DEBUG_VIEW_SHADED;
}

bool
Frames::set_view(int view) {
    if (!has_views()) {
        return false;
    }
    render_context_set_debug_view(context(), view);
    if (this->view() != view) {
        return false;
    }
    // Held (observing or placing), the checkpoint redraws the shown frame in one step; restoring keeps the view set here.
    return state_->held != nullptr || state_->observing ? redraw_unheld() : redraw_shown();
}

bool
Frames::step(uint32_t dt_ms) {
    release_hold();
    return advance(dt_ms);
}

bool
Frames::advance_stream() {
    State& s = *state_;
    if (s.index >= count() || (stream_next() != s.index && !stream_seek(s.index))) {
        return false;
    }
    const input_t input = {};
    render_host_step(stream_frame_info(s.index).dt_ms, &input);
    s.drawn++;
    s.index++;
    return true;
}

bool
Frames::advance(uint32_t dt_ms) {
    State& s = *state_;
    if (s.stream) {
        return advance_stream();
    }
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
    for (const auto& [name, local] : s.placed) {
        const entity_t entity = entity_named(name);
        if (!handle_is_none(entity)) {
            transform_set_local(entity, local);
        }
    }
    const input_t input = {};
    render_host_step(dt_ms, &input);
    s.drawn++;
    if (s.index == std::ssize(s.dts)) {
        s.dts.push_back(dt_ms);
    }
    s.index++;
    return saved;
}

bool
Frames::seek(int index) {
    if (state_->stream) {
        if (index < 0 || index >= count()) {
            return false;
        }
        release_hold();
        state_->index = index;
        return advance(0);
    }
    if (index < 0 || !redraw_unheld() || (index < state_->index - 1 && !restore_before(index))) {
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
    if (!restore_keeping_view(from->second)) {
        return false;
    }
    s.index = from->first;
    return true;
}

void
Frames::step_to(int index) {
    State& s = *state_;
    while (s.index <= index) {
        advance(s.index < std::ssize(s.dts) ? s.dts[s.index] : dt_ms());
    }
}

bool
Frames::redraw_shown() {
    const int shown = state_->index - 1;
    if (state_->stream) {
        state_->index = std::max(shown, 0);
        return shown < 0 || advance(0);
    }
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

bool
Frames::can_observe() const {
    const render_debug_view_t* row = render_context_debug_view(view());
    return row != nullptr && row->second_view != nullptr;
}

bool
Frames::observe(std::optional<transformf_t> pose) {
    const int shown = state_->index - 1;
    if (!can_observe() || shown < 0) {
        return false;
    }
    if (!pose) {
        return redraw_unheld();
    }
    render_context_set_observer(context(), &*pose);
    return redraw_held();
}

bool
Frames::redraw_held() {
    State& s = *state_;
    const int shown = s.index - 1;
    if (shown < 0) {
        return true;
    }
    if (s.stream) {
        s.observing = true;
        s.index = shown;
        return advance(0);
    }
    if (s.held == nullptr) {
        // Once: back to just before the shown frame, and hold that state.
        if (!restore_before(shown)) {
            return false;
        }
        step_to(shown - 1);
        s.held = render_host_save();
        if (s.held == nullptr) {
            std::fprintf(stderr, "loupe: no checkpoint before frame %d to redraw it from\n", shown);
            step_to(shown);
            return false;
        }
    } else if (restore_keeping_view(s.held)) {
        s.index = shown;
    } else {
        return false;
    }
    return advance(s.dts[shown]);
}

bool
Frames::redraw_unheld() {
    State& s = *state_;
    if (s.stream) {
        if (!s.observing) {
            return true;
        }
        release_hold();
        s.index--;
        return advance(0);
    }
    if (s.held == nullptr) {
        return true;
    }
    const int shown = s.index - 1;
    const bool restored = restore_keeping_view(s.held);
    render_context_set_observer(context(), nullptr);
    if (restored) {
        s.index = shown;
    }
    const bool drew = restored && advance(s.dts[shown]);
    release_hold();
    return drew;
}

std::optional<transformf_t>
Frames::observer_pose() const {
    if (!can_observe()) {
        return std::nullopt;
    }
    const render_context_t* drew = context();
    transformf_t pose;
    if (render_context_observer_pose(drew, &pose)) {
        return pose;
    }
    if (!drew->kept) {
        return std::nullopt;
    }
    return render_view_pose(&drew->kept_view);
}

box3f_t
Frames::scene_box() const {
    const entity_t camera = camera_active();
    return handle_is_none(camera) ? box3f_empty() : scene_bounds(camera);
}

bool
Frames::place(const std::string& object, const transformf_t& local) {
    const entity_t entity = entity_named(object);
    if (handle_is_none(entity)) {
        return false;
    }
    state_->placed[object] = local;
    transform_set_local(entity, local);
    // The redraw sets every placement again before its frame.
    return redraw_held();
}

std::optional<mat4f_t>
Frames::parent_world(const std::string& object) const {
    const entity_t entity = entity_named(object);
    if (handle_is_none(entity)) {
        return std::nullopt;
    }
    return transform_world_matrix(entity_parent(entity));
}

std::optional<ViewCamera>
Frames::view_camera() const {
    const entity_t camera = camera_active();
    const Frame shown = frame();
    if (handle_is_none(camera) || shown.width <= 0) {
        return std::nullopt;
    }
    const lens_t* lens = camera_lens(camera);
    const transformf_t pose = transform_world(camera);
    // The shown frame is the picture turned back upright from the panel, so its axes are the camera's own: quarter 0.
    render_view_t view = render_view_make(&pose, lens->half_fov_short_tan, lens->near_z,
                                          viewport_t{.width = shown.width, .height = shown.height, .quarter = 0});
    render_view_set_shift(&view, lens->shift);
    return ViewCamera{.eye = view.position,
                      .right = vec3f_normalize(view.screen_x),
                      .up = vec3f_scale(vec3f_normalize(view.screen_y), -1.0F),
                      .forward = vec3f_normalize(view.forward),
                      .fx = view.lens.fx,
                      .fy = view.lens.fy,
                      .cx = view.lens.cx,
                      .cy = view.lens.cy,
                      .near_z = view.lens.near_z,
                      .width = shown.width,
                      .height = shown.height};
}

int64_t
Frames::drawn() const {
    return state_->drawn;
}

void
Frames::release_hold() {
    State& s = *state_;
    if (s.held == nullptr && !s.observing) {
        return;
    }
    render_context_set_observer(context(), nullptr);
    if (s.held != nullptr) {
        render_host_checkpoint_free(s.held);
    }
    s.held = nullptr;
    s.observing = false;
}

int
Frames::first_seekable() const {
    return state_->stream ? 0 : FIRST_SEEKABLE;
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
    if (provider->path_ms == nullptr) {
        return std::nullopt;
    }
    return provider->path_ms();
}

bool
Frames::live() const {
    return state_->live;
}

bool
Frames::feed(std::span<const uint8_t> bytes) {
    return state_->live && stream_feed(bytes.data(), bytes.size());
}

std::optional<Frames::StreamState>
Frames::stream_state() const {
    if (!state_->stream) {
        return std::nullopt;
    }
    const stream_status_t status = stream_status();
    return StreamState{
        .frames = status.frames, .dropped = status.dropped, .refused = status.refused, .why = status.why};
}

Frames::Check
Frames::check(int index) const {
    if (!state_->stream) {
        return Check::None;
    }
    switch (stream_frame_info(index).check) {
        case STREAM_CHECK_SAME: return Check::Same;
        case STREAM_CHECK_DIFFERS: return Check::Differs;
        case STREAM_CHECK_OTHER_SIZE: return Check::OtherSize;
        case STREAM_CHECK_NONE: break;
    }
    return Check::None;
}

int
frames_on_path(uint32_t path_ms, uint32_t dt_ms) {
    return static_cast<int>(std::min<int64_t>(int64_t{path_ms / dt_ms} + 1, INT_MAX));
}

std::unique_ptr<Frames>
open_whole_path(const Options& options, std::string* why) {
    std::unique_ptr<Frames> frames = Frames::open(options, why);
    if (frames == nullptr || options.frames) {
        return frames;
    }
    const std::optional<uint32_t> path_ms = frames->path_ms();
    if (!path_ms || frames->dt_ms() == 0) {
        return frames;
    }
    // The run's length is fixed at open, and the path's is known only once open.
    const int whole = frames_on_path(*path_ms, frames->dt_ms());
    if (whole <= frames->count()) {
        return frames;
    }
    frames.reset();
    Options run = options;
    run.frames = whole;
    return Frames::open(run, why);
}

int64_t
Frames::video_frames_fit() const {
    return render_host_video_frames_fit();
}

bool
Frames::close() {
    if (!state_->open) {
        return true;
    }
    // The image close writes is the last drawn: the straight run's, not the observer's.
    const bool redrawn = redraw_unheld();
    state_->forget_after(-1);
    state_->open = false;
    return render_host_close() && redrawn;
}

bool
render(const Options& options, std::string* why) {
    const std::unique_ptr<Frames> frames = Frames::open(options, why);
    if (frames == nullptr) {
        return false;
    }
    const int count = frames->count();
    for (int i = 0; i < count; i++) {
        frames->step(frames->dt_ms());
    }
    if (!frames->close()) {
        *why = "the run failed; autana said why above";
        return false;
    }
    return true;
}

} // namespace loupe
