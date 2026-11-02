#include "window/files.hpp"

#include <SDL3/SDL.h>

#include <cstdio>
#include <mutex>
#include <utility>

#include "library.hpp"
#include "pack.hpp"

namespace loupe::window {

namespace {

// SDL_GetPrefPath's organization and application: the config folder's two names.
constexpr const char CONFIG_ORG[] = "autana";
constexpr const char CONFIG_APP[] = "loupe";

// SDL reads the filters until the callback, so they live as long as the program.
// Extensions without the dot, ";" between them.
constexpr SDL_DialogFileFilter PACKS[] = {{.name = "Scene packs", .pattern = "apak"}};
constexpr SDL_DialogFileFilter PACKS_AND_SCENES[] = {{.name = "Scenes", .pattern = "apak;toml"}};

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
    if (builds_packs()) {
        SDL_ShowOpenFileDialog(on_picked, nullptr, window, PACKS_AND_SCENES, SDL_arraysize(PACKS_AND_SCENES), nullptr,
                               false);
    } else {
        SDL_ShowOpenFileDialog(on_picked, nullptr, window, PACKS, SDL_arraysize(PACKS), nullptr, false);
    }
}

std::optional<std::filesystem::path>
browsed() {
    const std::scoped_lock hold(pending.lock);
    return std::exchange(pending.picked, std::nullopt);
}

Folders
folders() {
    Folders found;
    if (const char* base = SDL_GetBasePath()) {
        found.exe_dir = from_utf8(base);
    }
    if (char* pref = SDL_GetPrefPath(CONFIG_ORG, CONFIG_APP)) {
        found.config = from_utf8(pref);
        SDL_free(pref);
    }
    return found;
}

} // namespace loupe::window
