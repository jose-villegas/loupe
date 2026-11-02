#include "scratch.hpp"

#include <format>
#include <random>
#include <system_error>

namespace fs = std::filesystem;

namespace loupe {

Scratch::Scratch() {
    std::error_code error;
    const fs::path folder = fs::temp_directory_path(error) / std::format("loupe-{:08x}", std::random_device{}());
    if (!error && fs::create_directory(folder, error)) {
        path_ = folder;
    }
}

Scratch::~Scratch() {
    std::error_code ignored;
    if (!path_.empty()) {
        fs::remove_all(path_, ignored);
    }
}

} // namespace loupe
