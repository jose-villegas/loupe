// The docs' pictures of the window, each made by a scripted run instead of
// by hand: the scene and camera it opens, the state it drives the editor to
// through the editor's own calls, and the panel it keeps. `loupe --shot NAME
// -o OUT.png` writes one; the doc_shots target writes them all into docs/,
// and the doc_shots_current test fails when one there is not what its row draws.
#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>

#include "engine/frames.hpp"
#include "window/editor.hpp"
#include "window/gpu.hpp"

namespace loupe::window {

// What a shot drives once its scene is open: the editor, the run, and the timeline's seek.
struct ShotState {
    Editor& editor;
    const Frames& frames;
    std::optional<int>& seek_to;
};

struct Shot {
    std::string_view name;   // docs/<name>.png
    std::string_view scene;  // a .scene.toml, from autana's checkout
    std::string_view camera; // the camera looked through; the scene's first when empty
    // Sets the shot's state; false when the scene is not what the row expects.
    bool (*drive)(const ShotState& state);
    std::string_view crop; // a panel's window name; the whole window when empty
};

// The panels' scale in a shot, whatever the display's: a 150% desktop's, so
// the text stays readable where a page shows the picture smaller.
constexpr float SHOT_CONTENT_SCALE = 1.5F;
// How far apart a shot's frames are, so a seek along a path steps few frames.
constexpr uint32_t SHOT_DT_MS = 100;
// Ticks a shot runs after it is driven, before its picture is read: the panels
// dock on the first, and a seek, a fly or an upload lands on the next.
constexpr int SHOT_SETTLE_TICKS = 4;

// Every shot, in the order docs/editor/ shows them.
std::span<const Shot> doc_shots();
// The shot called `name`; null for none.
const Shot* find_shot(std::string_view name);
// `picture` as a PNG at `out`, cropped to `shot`'s panel where the last frame
// laid it out; false with `why` set when the panel is not there or the file
// cannot be written.
bool write_shot(const Picture& picture, const Shot& shot, const std::filesystem::path& out, std::string* why);

} // namespace loupe::window
