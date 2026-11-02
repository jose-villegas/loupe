#include "paths.hpp"

#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace loupe {

std::string
utf8(const fs::path& path) {
    const std::u8string text = path.u8string();
    return {text.begin(), text.end()};
}

fs::path
from_utf8(std::string_view text) {
    return {std::u8string(text.begin(), text.end())};
}

std::string
read_text(const std::filesystem::path& file) {
    const std::ifstream in(file, std::ios::binary);
    std::ostringstream text;
    text << in.rdbuf();
    return text.str();
}

} // namespace loupe
