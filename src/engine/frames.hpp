// gfx's frame from a provider, drawn by autana's host frame loop; step and
// exact seek. One Frames is open at a time: the loop is autana's one run.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
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

// The camera the shown frame was drawn through, in the frame's pixels: its
// eye and unit axes (right, picture-up, forward) in world space, and its lens.
struct ViewCamera {
    vec3f_t eye, right, up, forward;
    float fx = 0, fy = 0, cx = 0, cy = 0, near_z = 0;
    int width = 0, height = 0;
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
    // autana's stream provider on a frame record stream: `options.scene`'s
    // .framerec, or with no scene a live stream fed through feed(). Its packs
    // load by name from `packs`, and one whose crc differs refuses the stream.
    // `id` is what scene_id() says. Seeks need no checkpoints: the provider
    // decodes any frame from the keyframe before it.
    static std::unique_ptr<Frames> open_stream(const Options& options, const std::filesystem::path& packs,
                                               std::string id, std::string* why);
    ~Frames();
    Frames(const Frames&) = delete;
    Frames& operator=(const Frames&) = delete;
    Frames(Frames&&) = delete;
    Frames& operator=(Frames&&) = delete;

    // The scene's id: its pack's file name without .apak.
    const std::string& scene_id() const;
    // The run's frame count and dt as it opened: the options', else the provider's defaults.
    // A stream's count is its whole frames so far, and its dt the next frame's.
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
    // The world box around what the shown frame's scene draws (scene_bounds);
    // empty with no camera drawing. An observer's start speed is read from it.
    box3f_t scene_box() const;
    // Places the scene object named `object` at `local` (relative to its
    // parent) from here on: kept across steps and seeks, as every frame is
    // drawn with it, and the shown frame is drawn again with it now. False when
    // the scene holds no such object or the redraw failed.
    bool place(const std::string& object, const transformf_t& local);
    // The world matrix of `object`'s parent (the scene root for a top-level
    // object); none when the scene holds no such object.
    std::optional<mat4f_t> parent_world(const std::string& object) const;
    // The active camera as the shown frame drew it, its axes the frame's own on
    // a turned panel too; none without a camera.
    std::optional<ViewCamera> view_camera() const;
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
    // The first frame seek() reaches: FIRST_SEEKABLE for a scene, 0 for a stream.
    int first_seekable() const;
    // How many frames are drawn; frame() is frame index() - 1.
    int index() const;
    Frame frame() const;
    // The camera path's length; none without a path.
    std::optional<uint32_t> path_ms() const;
    // The most frames of this run's size one --video holds.
    int64_t video_frames_fit() const;
    // Writes what -o and --video ask for; false when the run failed.
    bool close();

    // A stream's state: whole frames, frames after a drop on the board, and
    // why the stream was refused when it was.
    struct StreamState {
        int frames = 0, dropped = 0;
        bool refused = false;
        std::string why;
    };
    // How a drawn frame compares with the CHECK the stream holds for it.
    enum class Check : uint8_t { None, Same, Differs, OtherSize };

    // True for a stream fed live: its count grows as bytes arrive.
    bool live() const;
    // A live stream's bytes as they arrive, in any pieces; false once it is refused.
    bool feed(std::span<const uint8_t> bytes);
    // The stream's state; none for a scene.
    std::optional<StreamState> stream_state() const;
    // Frame `index`'s CHECK verdict, as of its last draw; None for a scene.
    Check check(int index) const;

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
    // advance() for a stream: the provider draws the frame stream_next()
    // names, at its recorded dt.
    bool advance_stream();
    // Draws the shown frame again in one step from a checkpoint held just
    // before it; the first call takes the hold, replaying from the kept
    // checkpoint before. What observe() and place() redraw with, so a drag
    // costs a frame per move. True when nothing is drawn yet.
    bool redraw_held();
    // Frees the held checkpoint and clears any observer; nothing is drawn.
    void release_hold();
    // Draws the shown frame again from the held checkpoint without any observer,
    // one step, then frees the hold; true when nothing is held.
    bool redraw_unheld();
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
