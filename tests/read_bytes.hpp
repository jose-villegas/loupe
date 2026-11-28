// A file's whole contents, for tests that compare what was written.
#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

namespace loupe::test {

// `file`'s bytes; empty when it cannot be read.
inline std::vector<uint8_t>
read_bytes(const std::filesystem::path& file) {
    std::ifstream in(file, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

} // namespace loupe::test
