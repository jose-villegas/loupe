#include "window/doc_shots.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <memory>
#include <string>
#include <vector>

#include "editor/camera_path.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "paths.hpp"
#include "window/observer.hpp"
#include "window/path_editor.hpp"

namespace loupe::window {

namespace {

constexpr std::string_view CAPYBARA = "launcher/demo/capybara/capybara.scene.toml";
constexpr std::string_view SPONZA = "launcher/demo/sponza/sponza.scene.toml";
constexpr std::string_view BISTRO = "launcher/demo/bistro/bistro.scene.toml";
// Sponza's tour: its key the path shots pick and move, across the atrium
// (its eye's x, from -300), and the time the before/after pair is seen at.
constexpr std::string_view TOUR = "tour";
constexpr size_t TOUR_KEY = 3;
constexpr float TOUR_KEY_EYE_X = 500.0F;
constexpr float TOUR_SEEN_AT_S = 18.0F;
// The free view flies back out of the atrium from the scene's first camera, in
// steps of the observer's flight, until the whole tour is in sight.
constexpr float FREE_VIEW_BACK_S = 2.0F;
constexpr float FREE_VIEW_STEP_S = 0.1F;
constexpr std::string_view VIEWPORT = "Viewport";

bool
select(const ShotState& state, std::string_view object) {
    state.editor.selected = std::string(object);
    return state.editor.docs.scene.find(state.editor.selected) != nullptr;
}

// The tour selected and its key picked, the handle on its eye.
bool
pick_tour_key(const ShotState& state) {
    if (!select(state, TOUR) || selected_path(state.editor) == nullptr) {
        return false;
    }
    state.editor.path.key = KeyPick{.index = TOUR_KEY, .point = editor::KeyPoint::Eye, .camera = std::string(TOUR)};
    return shown_keys(state.editor, state.editor.selected).size() > TOUR_KEY;
}

// The picked key's eye moved, as a handle's drag ends: one unsaved edit.
bool
move_tour_key(const ShotState& state) {
    if (!pick_tour_key(state) || !may_edit(state.editor, state.editor.selected)) {
        return false;
    }
    std::vector<editor::CameraKey> keys = shown_keys(state.editor, state.editor.selected);
    keys[TOUR_KEY].eye[0] = TOUR_KEY_EYE_X;
    drag_keys(state.editor, state.editor.selected, std::move(keys));
    end_key_drag(state.editor);
    return state.editor.edits.can_undo();
}

// Free view on, flown straight back for FREE_VIEW_BACK_S.
bool
fly_back(const ShotState& state) {
    if (!start_free_view(state.editor, state.frames)) {
        return false;
    }
    const fly_motion_steer_t back = {.move = {.x = 0.0F, .y = 0.0F, .z = -1.0F}, .look = {}, .wheel = 0.0F};
    for (float flown = 0.0F; flown < FREE_VIEW_BACK_S; flown += FREE_VIEW_STEP_S) {
        (void)fly_motion_update(&state.editor.path.free.fly, &back, FREE_VIEW_STEP_S);
    }
    return true;
}

// The frame at TOUR_SEEN_AT_S on the tour's path as shown, seen clean: nothing
// selected, so no path is drawn over it.
bool
seek_tour(const ShotState& state) {
    const std::string tour(TOUR);
    if (!state.editor.docs.paths.contains(tour)) {
        return false;
    }
    state.seek_to = frame_at(shown_keys(state.editor, tour), TOUR_SEEN_AT_S,
                             {.shown_ms = state.editor.path.shown_ms, .dt_ms = SHOT_DT_MS});
    state.editor.selected.clear();
    state.editor.path.key.reset();
    return true;
}

const std::array SHOTS = {
    Shot{.name = "editor",
         .scene = CAPYBARA,
         .camera = "",
         .drive = [](const ShotState& state) { return select(state, "capybara"); },
         .crop = ""},
    Shot{.name = "editor-baked",
         .scene = CAPYBARA,
         .camera = "",
         .drive = [](const ShotState& state) { return select(state, "meadow"); },
         .crop = ""},
    Shot{.name = "path-keys",
         .scene = SPONZA,
         .camera = "",
         .drive = [](const ShotState& state) { return select(state, TOUR); },
         .crop = ""},
    Shot{.name = "key-picked", .scene = SPONZA, .camera = "", .drive = pick_tour_key, .crop = ""},
    Shot{.name = "free-view",
         .scene = SPONZA,
         .camera = "",
         .drive = [](const ShotState& state) { return select(state, TOUR) && fly_back(state); },
         .crop = ""},
    Shot{.name = "key-moved",
         .scene = SPONZA,
         .camera = "",
         .drive = [](const ShotState& state) { return fly_back(state) && move_tour_key(state); },
         .crop = ""},
    Shot{.name = "tour-before", .scene = SPONZA, .camera = TOUR, .drive = seek_tour, .crop = VIEWPORT},
    Shot{.name = "tour-after",
         .scene = SPONZA,
         .camera = TOUR,
         .drive = [](const ShotState& state) { return move_tour_key(state) && seek_tour(state); },
         .crop = VIEWPORT},
    Shot{.name = "bake-warning",
         .scene = BISTRO,
         .camera = "",
         .drive = [](const ShotState& state) { return select(state, "camera"); },
         .crop = ""},
};

} // namespace

std::span<const Shot>
doc_shots() {
    return SHOTS;
}

const Shot*
find_shot(std::string_view name) {
    const auto found = std::ranges::find(SHOTS, name, &Shot::name);
    return found == SHOTS.end() ? nullptr : &*found;
}

bool
write_shot(const Picture& picture, const Shot& shot, const std::filesystem::path& out, std::string* why) {
    int x = 0, y = 0, width = picture.width, height = picture.height;
    if (!shot.crop.empty()) {
        const ImGuiWindow* panel = ImGui::FindWindowByName(std::string(shot.crop).c_str());
        if (panel == nullptr) {
            *why = std::format("no panel {}", shot.crop);
            return false;
        }
        const ImVec2 scale = ImGui::GetIO().DisplayFramebufferScale;
        x = std::clamp(static_cast<int>(panel->Pos.x * scale.x), 0, picture.width);
        y = std::clamp(static_cast<int>(panel->Pos.y * scale.y), 0, picture.height);
        width = std::clamp(static_cast<int>(panel->Size.x * scale.x), 0, picture.width - x);
        height = std::clamp(static_cast<int>(panel->Size.y * scale.y), 0, picture.height - y);
    }
    const int pitch = picture.width * Picture::CHANNELS;
    // The surface only points into the picture: a const view SDL does not write through.
    auto* first = const_cast<uint8_t*>(picture.rgba.data()) + (static_cast<ptrdiff_t>(y) * pitch)
                  + (static_cast<ptrdiff_t>(x) * Picture::CHANNELS);
    const std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)> surface(
        SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, first, pitch), &SDL_DestroySurface);
    if (surface == nullptr || !SDL_SavePNG(surface.get(), utf8(out).c_str())) {
        *why = std::format("cannot write {}: {}", utf8(out), SDL_GetError());
        return false;
    }
    return true;
}

} // namespace loupe::window
