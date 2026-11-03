#include "update/sha256.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <fstream>

namespace loupe::update {

namespace {

constexpr size_t BLOCK = 64;
constexpr size_t LENGTH_BYTES = 8;
constexpr size_t READ_CHUNK = size_t{1} << 16;

constexpr std::array<uint32_t, 64> K = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

// The running state over whole blocks; the tail waits for more bytes or the end.
class Hasher {
  public:
    void
    add(std::string_view bytes) {
        total_ += bytes.size();
        for (const char c : bytes) {
            block_[used_++] = static_cast<uint8_t>(c);
            if (used_ == BLOCK) {
                compress();
                used_ = 0;
            }
        }
    }

    std::string
    finish() {
        const uint64_t bits = total_ * 8;
        add(std::string_view("\x80", 1));
        while (used_ != BLOCK - LENGTH_BYTES) {
            add(std::string_view("\0", 1));
        }
        for (size_t i = 0; i < LENGTH_BYTES; i++) {
            const char byte = static_cast<char>((bits >> (8 * (LENGTH_BYTES - 1 - i))) & 0xff);
            add(std::string_view(&byte, 1));
        }
        constexpr const char HEX[] = "0123456789abcdef";
        std::string out;
        for (const uint32_t word : h_) {
            for (int shift = 28; shift >= 0; shift -= 4) {
                out += HEX[(word >> shift) & 0xf];
            }
        }
        return out;
    }

  private:
    void
    compress() {
        std::array<uint32_t, 64> w{};
        for (size_t i = 0; i < 16; i++) {
            w[i] = (uint32_t{block_[4 * i]} << 24) | (uint32_t{block_[4 * i + 1]} << 16)
                   | (uint32_t{block_[4 * i + 2]} << 8) | uint32_t{block_[4 * i + 3]};
        }
        for (size_t i = 16; i < w.size(); i++) {
            const uint32_t s0 = std::rotr(w[i - 15], 7) ^ std::rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            const uint32_t s1 = std::rotr(w[i - 2], 17) ^ std::rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }
        std::array<uint32_t, 8> v = h_;
        for (size_t i = 0; i < w.size(); i++) {
            const uint32_t s1 = std::rotr(v[4], 6) ^ std::rotr(v[4], 11) ^ std::rotr(v[4], 25);
            const uint32_t choice = (v[4] & v[5]) ^ (~v[4] & v[6]);
            const uint32_t t1 = v[7] + s1 + choice + K[i] + w[i];
            const uint32_t s0 = std::rotr(v[0], 2) ^ std::rotr(v[0], 13) ^ std::rotr(v[0], 22);
            const uint32_t majority = (v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]);
            v = {t1 + s0 + majority, v[0], v[1], v[2], v[3] + t1, v[4], v[5], v[6]};
        }
        for (size_t i = 0; i < h_.size(); i++) {
            h_[i] += v[i];
        }
    }

    std::array<uint32_t, 8> h_ = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                                  0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    std::array<uint8_t, BLOCK> block_{};
    size_t used_ = 0;
    uint64_t total_ = 0;
};

} // namespace

std::string
sha256(std::string_view bytes) {
    Hasher hasher;
    hasher.add(bytes);
    return hasher.finish();
}

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

} // namespace loupe::update
