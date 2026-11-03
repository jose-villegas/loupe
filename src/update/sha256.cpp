#include "update/sha256.hpp"

#include <array>
#include <cstddef>
#include <format>
#include <fstream>
#include <string_view>

#include <mbedtls/sha256.h>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace loupe::update {

namespace {

constexpr size_t SHA256_BYTES = 32;
constexpr size_t READ_CHUNK = size_t{1} << 16;

// Mbed TLS's SHA-256 over bytes added in pieces.
class Hasher {
  public:
    Hasher() {
        mbedtls_sha256_init(&context_);
        ok_ = mbedtls_sha256_starts(&context_, 0) == 0;
    }

    ~Hasher() { mbedtls_sha256_free(&context_); }

    Hasher(const Hasher&) = delete;
    Hasher& operator=(const Hasher&) = delete;
    Hasher(Hasher&&) = delete;
    Hasher& operator=(Hasher&&) = delete;

    void
    add(std::string_view bytes) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast): Mbed TLS takes bytes as unsigned char.
        const auto* data = reinterpret_cast<const unsigned char*>(bytes.data());
        ok_ = ok_ && mbedtls_sha256_update(&context_, data, bytes.size()) == 0;
    }

    // The digest as 64 lower-case hex digits; empty when hashing failed.
    std::string
    finish() {
        std::array<unsigned char, SHA256_BYTES> digest{};
        if (!ok_ || mbedtls_sha256_finish(&context_, digest.data()) != 0) {
            return {};
        }
        std::string hex;
        for (const unsigned char byte : digest) {
            hex += std::format("{:02x}", byte);
        }
        return hex;
    }

  private:
    mbedtls_sha256_context context_{};
    bool ok_ = false;
};

} // namespace

std::string
sha256_file(const std::filesystem::path& file) {
    std::ifstream in(file, std::ios::binary);
    if (!in) {
        return {};
    }
    Hasher hasher;
    std::string chunk(READ_CHUNK, '\0');
    while (in.read(chunk.data(), static_cast<std::streamsize>(chunk.size())) || in.gcount() > 0) {
        hasher.add(std::string_view(chunk.data(), static_cast<size_t>(in.gcount())));
    }
    return in.bad() ? std::string() : hasher.finish();
}

bool
run_verified(const std::filesystem::path& file, const std::string& sha256, const std::function<bool()>& run) {
#if defined(_WIN32)
    // Shared for reading only: no writer, rename or delete while it is open.
    const HANDLE held = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (held == INVALID_HANDLE_VALUE) {
        return false;
    }
    Hasher hasher;
    std::string chunk(READ_CHUNK, '\0');
    DWORD got = 0;
    bool read = true;
    while ((read = ReadFile(held, chunk.data(), static_cast<DWORD>(chunk.size()), &got, nullptr) != FALSE) && got > 0) {
        hasher.add(std::string_view(chunk.data(), got));
    }
    const bool ran = read && hasher.finish() == sha256 && run();
    CloseHandle(held);
    return ran;
#else
    return sha256_file(file) == sha256 && run();
#endif
}

} // namespace loupe::update
