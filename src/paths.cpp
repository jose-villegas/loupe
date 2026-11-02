#include "paths.hpp"

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

} // namespace loupe
