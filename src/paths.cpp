#include "paths.hpp"

#include <fstream>
#include <sstream>
#include <system_error>

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

std::vector<std::string>
read_lines(const std::filesystem::path& file) {
    std::vector<std::string> lines;
    std::ifstream in(file, std::ios::binary);
    std::string line;
    while (std::getline(in, line)) {
        if (line.ends_with('\r')) {
            line.pop_back();
        }
        lines.push_back(line);
    }
    return lines;
}

bool
write_lines(const std::filesystem::path& file, const std::vector<std::string>& lines) {
    std::error_code error;
    fs::create_directories(file.parent_path(), error);
    const fs::path temp = fs::path(file) += ".tmp";
    {
        std::ofstream out(temp, std::ios::binary | std::ios::trunc);
        for (const std::string& line : lines) {
            out << line << '\n';
        }
        if (!out.good()) {
            return false;
        }
    }
    fs::rename(temp, file, error);
    return !error;
}

} // namespace loupe
