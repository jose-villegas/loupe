#include "update/sha256.hpp"

#include <cstddef>
#include <format>
#include <string_view>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
// After windows.h, which it needs.
#include <bcrypt.h>
#else
#include "paths.hpp"
#include "process.hpp"
#endif

namespace loupe::update {

namespace {

constexpr size_t SHA256_BYTES = 32;

#if defined(_WIN32)
constexpr DWORD READ_CHUNK = DWORD{1} << 16;

// The digest of what is left to read from `file`; empty when it cannot be read.
std::string
hash_handle(HANDLE file) {
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0))) {
        return {};
    }
    if (!BCRYPT_SUCCESS(BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0))) {
        BCryptCloseAlgorithmProvider(algorithm, 0);
        return {};
    }
    std::vector<unsigned char> chunk(READ_CHUNK);
    DWORD got = 0;
    bool read = true;
    while ((read = ReadFile(file, chunk.data(), READ_CHUNK, &got, nullptr) != FALSE) && got > 0) {
        BCryptHashData(hash, chunk.data(), got, 0);
    }
    std::vector<unsigned char> digest(SHA256_BYTES);
    const bool done = BCRYPT_SUCCESS(BCryptFinishHash(hash, digest.data(), SHA256_BYTES, 0));
    BCryptDestroyHash(hash);
    BCryptCloseAlgorithmProvider(algorithm, 0);
    std::string hex;
    for (const unsigned char byte : digest) {
        hex += std::format("{:02x}", byte);
    }
    return read && done ? hex : std::string();
}

// `file` opened for reading, shared for reading only.
HANDLE
open_held(const std::filesystem::path& file) {
    return CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                       nullptr);
}
#endif

} // namespace

std::string
sha256_file(const std::filesystem::path& file) {
#if defined(_WIN32)
    const HANDLE held = open_held(file);
    if (held == INVALID_HANDLE_VALUE) {
        return {};
    }
    std::string digest = hash_handle(held);
    CloseHandle(held);
    return digest;
#else
    const std::filesystem::path program = system_program("sha256sum");
    const std::filesystem::path said = std::filesystem::path(file) += ".sha256";
    const bool ran = !program.empty() && run_logged({utf8(program), "-b", utf8(file)}, said);
    // A line starts with a backslash when sha256sum had to escape the file's name.
    std::string line = ran ? read_text(said) : std::string();
    std::string digest = line.substr(line.starts_with('\\') ? 1 : 0, 2 * SHA256_BYTES);
    std::error_code error;
    std::filesystem::remove(said, error);
    return digest.size() == 2 * SHA256_BYTES ? digest : std::string();
#endif
}

bool
run_verified(const std::filesystem::path& file, const std::string& sha256, const std::function<bool()>& run) {
#if defined(_WIN32)
    // Shared for reading only: no writer, rename or delete while it is open.
    const HANDLE held = open_held(file);
    if (held == INVALID_HANDLE_VALUE) {
        return false;
    }
    const bool ran = hash_handle(held) == sha256 && run();
    CloseHandle(held);
    return ran;
#else
    return sha256_file(file) == sha256 && run();
#endif
}

} // namespace loupe::update
