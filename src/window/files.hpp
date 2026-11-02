// The window's files through SDL: the system's open-file dialog for a scene,
// and the folders the start panel reads. The dialog answers on its own
// thread, so the window asks for the answer each tick.
#pragma once

#include <filesystem>
#include <optional>

#include "library.hpp"

struct SDL_Window;

namespace loupe::window {

// Shows the dialog over `window` for the scene files this build opens; one at a time.
void browse(SDL_Window* window);
// The file the viewer picked since the last call, once.
std::optional<std::filesystem::path> browsed();
// The folder loupe's executable is in, and its settings folder, made if
// missing: %APPDATA%\autana\loupe or ~/.local/share/autana/loupe. Each
// empty when the system does not say.
Folders folders();

} // namespace loupe::window
