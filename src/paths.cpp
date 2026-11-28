#include "paths.hpp"

#include <chrono>
#include <fstream>
#include <sstream>
#include <system_error>
#include <thread>

namespace fs = std::filesystem;

namespace loupe {

namespace {

// How long write_lines retries replacing its file. On Windows, replacing a
// file written moments before now and then fails with access denied, and
// succeeds when tried again; Node's graceful-fs retries such renames too.
constexpr auto REPLACE_RETRY_FOR = std::chrono::seconds(1);
constexpr auto REPLACE_RETRY_EVERY = std::chrono::milliseconds(10);

} // namespace

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
    const auto give_up = std::chrono::steady_clock::now() + REPLACE_RETRY_FOR;
    fs::rename(temp, file, error);
    while (error && std::chrono::steady_clock::now() < give_up) {
        std::this_thread::sleep_for(REPLACE_RETRY_EVERY);
        fs::rename(temp, file, error);
    }
    return !error;
}

} // namespace loupe
