// gfx's frame from a provider, drawn by autana's host frame loop; step and
// exact seek. One Frames is open at a time: the loop is autana's one run.
#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>

#include "options.h"

namespace loupe {

// The last drawn frame, as the BMP body autana writes: bottom-up BGR rows.
using Frame = std::span<const uint8_t>;

class Frames {
public:
    // With checkpoints kept, the state before frames 1, 1 + this, 1 + twice this... is saved.
    static constexpr int CHECKPOINT_EVERY = 30;

    // The scene provider on `options`' scene file, its pack built first.
    static std::unique_ptr<Frames> open(const Options& options, std::string* why);
    ~Frames();
    Frames(const Frames&) = delete;
    Frames& operator=(const Frames&) = delete;

    // The run's frame count and dt as it opened: the options', else the provider's defaults.
    int count() const;
    uint32_t dt_ms() const;
    // Saves checkpoints from here on, so seek() can go back; a command-line run never does.
    void keep_checkpoints();
    // Draws frame index(), `dt_ms` after the one before it; false when a checkpoint could not be saved.
    bool step(uint32_t dt_ms);
    // Leaves frame() at frame `index`, exactly as stepping straight there drew it.
    // False back to a frame before the first checkpoint, or to frame 0, which autana
    // cannot restore. A seek re-draws frames, so do not mix it with -o or --video.
    bool seek(int index);
    // How many frames are drawn; frame() is frame index() - 1.
    int index() const;
    Frame frame() const;
    std::optional<uint32_t> path_ms() const;
    // Writes what -o and --video ask for; false when the run failed.
    bool close();

private:
    struct State;
    explicit Frames(std::unique_ptr<State> state);
    std::unique_ptr<State> state_;
};

}  // namespace loupe
