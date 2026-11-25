// same_image [--differ] [--within N] A B: exits 0 when the two images, each a
// .png or a .bmp, hold the same pixels, 1 when they differ; with --differ, the
// other way round. --within N lets each channel differ by up to N levels.
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <span>
#include <string>
#include <system_error>
#include <vector>

namespace {

constexpr const char TEST[] = "same_image";
constexpr const char DIFFER[] = "--differ";
constexpr const char WITHIN[] = "--within";

using Surface = std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)>;

// `path` as RGB24, or null, said on stderr, when it does not load.
Surface
load(const std::string& path) {
    const bool png = path.ends_with(".png");
    const Surface loaded(png ? SDL_LoadPNG(path.c_str()) : SDL_LoadBMP(path.c_str()), &SDL_DestroySurface);
    Surface rgb(loaded ? SDL_ConvertSurface(loaded.get(), SDL_PIXELFORMAT_RGB24) : nullptr, &SDL_DestroySurface);
    if (rgb == nullptr) {
        std::fprintf(stderr, "%s: cannot load %s: %s\n", TEST, path.c_str(), SDL_GetError());
    }
    return rgb;
}

// The surface's RGB24 rows, without the padding a row may end in.
std::vector<uint8_t>
pixels(const SDL_Surface& surface) {
    const auto row_bytes = static_cast<size_t>(surface.w) * 3;
    std::vector<uint8_t> packed;
    for (int y = 0; y < surface.h; y++) {
        const auto* row = static_cast<const uint8_t*>(surface.pixels) + static_cast<ptrdiff_t>(y) * surface.pitch;
        packed.insert(packed.end(), row, row + row_bytes);
    }
    return packed;
}

// True when `a` and `b` are the same size and no channel is more than `within` levels apart.
bool
alike(const SDL_Surface& a, const SDL_Surface& b, int within) {
    if (a.w != b.w || a.h != b.h) {
        return false;
    }
    const std::vector<uint8_t> left = pixels(a);
    const std::vector<uint8_t> right = pixels(b);
    return std::ranges::equal(left, right, [within](uint8_t l, uint8_t r) { return std::abs(l - r) <= within; });
}

} // namespace

int
main(int argc, char** argv) {
    const std::span<char*> args(argv, static_cast<size_t>(argc));
    bool differ = false;
    int within = 0;
    size_t at = 1;
    bool ok = true;
    for (; ok && at + 2 < args.size(); at++) {
        const std::string flag = args[at];
        if (flag == DIFFER) {
            differ = true;
        } else if (flag == WITHIN && at + 3 < args.size()) {
            const std::string level = args[++at];
            const auto [end, error] = std::from_chars(level.data(), level.data() + level.size(), within);
            ok = error == std::errc() && end == level.data() + level.size() && within >= 0;
        } else {
            ok = false;
        }
    }
    if (!ok || at + 2 != args.size()) {
        std::fprintf(stderr, "usage: %s [%s] [%s N] A B\n", TEST, DIFFER, WITHIN);
        return 2;
    }
    const Surface a = load(args[at]);
    const Surface b = load(args[at + 1]);
    if (a == nullptr || b == nullptr) {
        return 2;
    }
    const bool equal = alike(*a, *b, within);
    std::fprintf(stderr, "%s: %s and %s %s\n", TEST, args[at], args[at + 1], equal ? "hold the same pixels" : "differ");
    return equal != differ ? 0 : 1;
}
