// gfx's frame from a provider, drawn by autana's host frame loop; step and
// exact seek. One Frames is open at a time: the loop is autana's one run.
#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "options.h"

struct render_host_checkpoint;  // render_host.h

namespace loupe {

// The last drawn frame, as the BMP body autana writes: bottom-up BGR rows.
struct Frame {
    const uint8_t* pixels;
    size_t bytes;
};

class Frames {
public:
    // Play saves the state before frames 1, 1 + this, 1 + twice this...; a seek steps at most this many.
    static constexpr int CHECKPOINT_EVERY = 30;

    // The scene provider on `options`' scene file, its pack built first.
    static std::unique_ptr<Frames> open(const Options& options, std::string* why);
    ~Frames();
    Frames(const Frames&) = delete;
    Frames& operator=(const Frames&) = delete;

    // Draws frame index(), `dt_ms` after the one before it.
    void step(uint32_t dt_ms);
    // Leaves frame() at frame `index`, exactly as stepping straight there drew it.
    // Back to frame 0 is false: autana cannot restore the state before the first frame.
    bool seek(int index);
    // How many frames are drawn; frame() is frame index() - 1.
    int index() const { return index_; }
    Frame frame() const;
    std::optional<uint32_t> path_ms() const;
    // Writes what -o and --video ask for; false when the run failed.
    bool close();

private:
    Frames() = default;
    void drop_checkpoints_after(int index);

    std::vector<std::string> args_;
    std::vector<char*> argv_;
    uint32_t dt_ms_ = 0;  // a seek past the drawn frames steps at this
    int index_ = 0;
    std::vector<uint32_t> dts_;  // each drawn frame's dt, for replaying one
    std::map<int, render_host_checkpoint*> checkpoints_;  // the state before drawing frame key
    bool open_ = false;
};

}  // namespace loupe
