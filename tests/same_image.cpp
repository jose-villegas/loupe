// same_image A B: exits 0 when the two images, each a .png or a .bmp, hold the
// same pixels, 1 when they differ; with --differ, the other way round.
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_surface.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace {

constexpr const char TEST[] = "same_image";
constexpr const char DIFFER[] = "--differ";

using Surface = std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)>;

// `path` as RGB24, or null, said on stderr, when it does not load.
Surface
load(const std::string& path) {
    const bool png = path.ends_with(".png");
    Surface loaded(png ? SDL_LoadPNG(path.c_str()) : SDL_LoadBMP(path.c_str()), &SDL_DestroySurface);
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

} // namespace

int
main(int argc, char** argv) {
    const std::span<char*> args(argv, static_cast<size_t>(argc));
    const bool differ = args.size() == 4 && std::string(args[1]) == DIFFER;
    if (args.size() != (differ ? 4U : 3U)) {
        std::fprintf(stderr, "usage: %s [%s] A B\n", TEST, DIFFER);
        return 2;
    }
    const Surface a = load(args[args.size() - 2]);
    const Surface b = load(args[args.size() - 1]);
    if (a == nullptr || b == nullptr) {
        return 2;
    }
    const bool equal = a->w == b->w && a->h == b->h && pixels(*a) == pixels(*b);
    std::fprintf(stderr, "%s: %s and %s %s\n", TEST, args[args.size() - 2], args[args.size() - 1],
                 equal ? "hold the same pixels" : "differ");
    return equal != differ ? 0 : 1;
}
