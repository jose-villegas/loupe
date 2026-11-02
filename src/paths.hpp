// A path as UTF-8 text and back: the spelling of recent.txt, ImGui and SDL.
#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace loupe {

std::string utf8(const std::filesystem::path& path);
std::filesystem::path from_utf8(std::string_view text);

} // namespace loupe
