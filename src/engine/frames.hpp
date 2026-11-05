// gfx's frame from a provider, drawn by autana's host frame loop; step and
// exact seek. One Frames is open at a time: the loop is autana's one run.
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "engine/pose.hpp"
#include "options.hpp"

namespace loupe {

// The last drawn frame, as the BMP body autana writes: bottom-up BGR rows, `stride` bytes apart.
struct Frame {
    std::span<const uint8_t> bgr;
    int width = 0, height = 0;
    size_t stride = 0;
};

// One row of autana's debug-view table; id 0 is the shaded picture.
struct DebugView {
    int id = 0;
    std::string name;
};

// Every view a render context can draw, shaded first, in autana's table order.
std::vector<DebugView> debug_views();

class Frames {
  public:
    // With checkpoints kept, the state before frames FIRST_SEEKABLE, FIRST_SEEKABLE + this... is saved,
    // until thinning doubles the interval.
    static constexpr int CHECKPOINT_EVERY = 30;
    // The first frame a seek reaches: the first checkpoint is saved before it, and
    // autana restores no state from before frame 1.
    static constexpr int FIRST_SEEKABLE = 1;

    // The scene provider on `options`' scene, through resolve_scene.
    static std::unique_ptr<Frames> open(const Options& options, std::string* why);
    ~Frames();
    Frames(const Frames&) = delete;
    Frames& operator=(const Frames&) = delete;
    Frames(Frames&&) = delete;
    Frames& operator=(Frames&&) = delete;

    // The scene's id: its pack's file name without .apak.
    const std::string& scene_id() const;
    // The run's frame count and dt as it opened: the options', else the provider's defaults.
    int count() const;
    uint32_t dt_ms() const;
    // Saves checkpoints from here on, so seek() can go back; a command-line run never does.
    // Past `budget_bytes` of them, every other one is dropped and the interval doubles.
    void keep_checkpoints(size_t budget_bytes);
    // The interval between kept checkpoints: CHECKPOINT_EVERY, doubled per thinning.
    int checkpoint_every() const;
    // The memory the kept checkpoints hold.
    size_t checkpoint_bytes() const;
    // True when the provider draws through a render context, so set_view() can switch views.
    bool has_views() const;
    // The view frames draw under now: a DebugView id.
    int view() const;
    // Draws under `view` from here on, and draws the shown frame again under it when a
    // checkpoint before it allows. Kept across seeks: a checkpoint holds the view it was
    // saved with, so every restore sets it again. Observing, the redraw stops it. False
    // without a render context.
    bool set_view(int view);
    // Whether the view draws a second view (render_debug_view_t.second_view), which observe() moves.
    bool can_observe() const;
    // Draws the shown frame again with the view's second view seen from `pose`,
    // or from the view's own spectator with none, which also stops observing.
    // The first call holds a checkpoint just before the shown frame, so each call
    // after it restores that and steps one frame. False when the view cannot
    // observe or the redraw failed.
    bool observe(std::optional<transformf_t> pose);
    // Where the shown frame's second view was seen from, or the camera's pose
    // when the view drew none: where an observer starts. None without a view
    // that can observe, or before a frame is drawn.
    std::optional<transformf_t> observer_pose() const;
    // Engine frames drawn since open, replays included: what seeks and observes cost.
    int64_t drawn() const;
    // Draws frame index(), `dt_ms` after the one before it; false when a checkpoint
    // could not be saved. Stops observing first: the next frame does not depend on it.
    bool step(uint32_t dt_ms);
    // Leaves frame() at frame `index`, exactly as stepping straight there drew it.
    // False back to a frame before the first checkpoint, or to frame 0, which autana
    // cannot restore. A seek re-draws frames, so do not mix it with -o or --video.
    // Observing, the shown frame is first drawn again without the observer.
    bool seek(int index);
    // How many frames are drawn; frame() is frame index() - 1.
    int index() const;
    Frame frame() const;
    // The camera path's length; none without a path.
    std::optional<uint32_t> path_ms() const;
    // The most frames of this run's size one --video holds.
    int64_t video_frames_fit() const;
    // Writes what -o and --video ask for; false when the run failed.
    bool close();

  private:
    struct State;
    // Restores the last checkpoint at or before frame `index`, keeping the picked view.
    bool restore_before(int index);
    // Steps until frame `index` is shown, each frame at the dt it drew with before.
    void step_to(int index);
    // Draws the shown frame again from the checkpoint before it, when there is one.
    bool redraw_shown();
    // step() without leaving the observer: draws frame index().
    bool advance(uint32_t dt_ms);
    // Frees the held checkpoint and clears the observer; nothing is drawn.
    void stop_observing();
    // Draws the shown frame again from the held checkpoint without the observer,
    // one step, then stops observing; true when not observing.
    bool leave_observing();
    explicit Frames(std::unique_ptr<State> state);
    std::unique_ptr<State> state_;
};

// The frames from a camera path's start to its end, `dt_ms` (> 0) apart.
int frames_on_path(uint32_t path_ms, uint32_t dt_ms);

// `options`' run over the whole camera path when --frames is not given and the
// path outlasts the provider's own short run; `options` as given otherwise,
// and for a still run (--dt 0), which has no path to cover.
// Null, with `why` set, when it cannot open.
std::unique_ptr<Frames> open_whole_path(const Options& options, std::string* why);

// One run as -o and --video ask: opened, stepped through every frame, closed;
// false with `why` set when it failed.
bool render(const Options& options, std::string* why);

} // namespace loupe
