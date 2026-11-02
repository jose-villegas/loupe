// A path as UTF-8 text and back: the spelling of recent.txt, ImGui and SDL;
// and a file read whole.
#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace loupe {

std::string utf8(const std::filesystem::path& path);
std::filesystem::path from_utf8(std::string_view text);
// The whole of `file` as bytes; empty when it cannot be read.
std::string read_text(const std::filesystem::path& file);

} // namespace loupe
