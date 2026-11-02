#include "window/files.hpp"

#include <SDL3/SDL.h>

#include <cstdio>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>

#include "pack.hpp"
#include "paths.hpp"
#include "process.hpp"

namespace loupe::window {

namespace {

// SDL_GetPrefPath's organization and application: the config folder's two names.
constexpr const char CONFIG_ORG[] = "autana";
constexpr const char CONFIG_APP[] = "loupe";

// SDL's filter pattern: extensions without the dot, ";" between them.
constexpr std::string_view SCENE_EXTENSION = ".toml";

// The scene files this build opens, as one dialog filter. SDL reads it until
// the callback, so it lives as long as the program.
const SDL_DialogFileFilter&
scene_filter() {
    static const std::string pattern =
        std::string(PACK_SUFFIX.substr(1)) + (builds_packs() ? ";" + std::string(SCENE_EXTENSION.substr(1)) : "");
    static const SDL_DialogFileFilter filter = {.name = "Scenes", .pattern = pattern.c_str()};
    return filter;
}

// What the dialog's thread hands the window's.
struct Pending {
    std::mutex lock;
    bool showing = false;
    std::optional<std::filesystem::path> picked;
};

Pending pending;

void SDLCALL
on_picked(void* /*userdata*/, const char* const* files, int /*filter*/) {
    const std::scoped_lock hold(pending.lock);
    pending.showing = false;
    if (files == nullptr) {
        std::fprintf(stderr, "loupe: no open dialog: %s\n", SDL_GetError());
    } else if (files[0] != nullptr) {
        pending.picked = from_utf8(files[0]);
    }
}

} // namespace

void
browse(SDL_Window* window) {
    {
        const std::scoped_lock hold(pending.lock);
        if (pending.showing) {
            return;
        }
        pending.showing = true;
    }
    SDL_ShowOpenFileDialog(on_picked, nullptr, window, &scene_filter(), 1, nullptr, false);
}

std::optional<std::filesystem::path>
browsed() {
    const std::scoped_lock hold(pending.lock);
    return std::exchange(pending.picked, std::nullopt);
}

Folders
folders() {
    Folders found = {.exe_dir = exe_path().parent_path(), .config = {}};
    if (char* pref = SDL_GetPrefPath(CONFIG_ORG, CONFIG_APP)) {
        found.config = from_utf8(pref);
        SDL_free(pref);
    }
    return found;
}

} // namespace loupe::window
