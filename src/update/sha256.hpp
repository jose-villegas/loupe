// SHA-256 (Mbed TLS's), for checking a download against GitHub's digest.
#pragma once

#include <filesystem>
#include <functional>
#include <string>

namespace loupe::update {

// `file`'s digest as 64 lower-case hex digits; empty when it cannot be read.
std::string sha256_file(const std::filesystem::path& file);
// Calls `run` when `file`'s digest is `sha256`, and says what it returned;
// false without calling it otherwise. On Windows the file is held open from
// the hash until `run` returns, and no one can write, rename or delete it
// meanwhile, so what `run` starts is what was hashed.
bool run_verified(const std::filesystem::path& file, const std::string& sha256, const std::function<bool()>& run);

} // namespace loupe::update
