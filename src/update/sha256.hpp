// SHA-256 (FIPS 180-4), for checking a download against GitHub's digest.
#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace loupe::update {

// `bytes`' digest as 64 lower-case hex digits.
std::string sha256(std::string_view bytes);
// `file`'s digest as 64 lower-case hex digits; empty when it cannot be read.
std::string sha256_file(const std::filesystem::path& file);

} // namespace loupe::update
