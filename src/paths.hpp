// A path as UTF-8 text and back: the spelling of recent.txt, ImGui and SDL;
// and a file read whole, read by line or written by line.
#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace loupe {

std::string utf8(const std::filesystem::path& path);
std::filesystem::path from_utf8(std::string_view text);
// The whole of `file` as bytes; empty when it cannot be read.
std::string read_text(const std::filesystem::path& file);
// `file`'s lines without their line ends, \n or \r\n; none when it cannot be read.
std::vector<std::string> read_lines(const std::filesystem::path& file);
// `lines` as `file`, each ended by \n, its folder made: written beside it,
// then renamed over it, so a reader finds the old file or the new, whole.
// False when it cannot be written.
bool write_lines(const std::filesystem::path& file, const std::vector<std::string>& lines);

} // namespace loupe
